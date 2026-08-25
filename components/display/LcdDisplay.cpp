#include "LcdDisplay.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr int kI2cMasterTimeoutMs = 1000;
constexpr uint8_t kLcdEnable = 0x04;
constexpr uint8_t kLcdRegisterSelect = 0x01;

}  // namespace

LcdDisplay::LcdDisplay(i2c_master_bus_handle_t bus_handle,
                       uint8_t address,
                       uint16_t frequency,
                       i2c_addr_bit_len_t address_length) {
    i2c_device_config_t dev_config = {};
    dev_config.dev_addr_length = address_length;
    dev_config.device_address = address;
    dev_config.scl_speed_hz = frequency;

    esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle_);
    if (err != ESP_OK) {
        ESP_LOGI("LcdDisplay", "Failed To Add Device: %d", address);
    }
}

esp_err_t LcdDisplay::WriteByte(uint8_t data) {
    const uint8_t payload = data | backlight_;
    return i2c_master_transmit(dev_handle_, &payload, 1, kI2cMasterTimeoutMs);
}

void LcdDisplay::ToggleEnable(uint8_t data) {
    WriteByte(data | kLcdEnable);
    WriteByte(data & ~kLcdEnable);
}

void LcdDisplay::SendNibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | backlight_;
    if (rs) {
        data |= kLcdRegisterSelect;
    }
    ToggleEnable(data);
}

void LcdDisplay::SendByte(uint8_t byte, uint8_t rs) {
    SendNibble(byte >> 4, rs);
    SendNibble(byte & 0x0F, rs);
}

void LcdDisplay::SendCommand(uint8_t cmd) {
    SendByte(cmd, 0);
}

void LcdDisplay::SendChar(char c) {
    SendByte(c, 1);
}

void LcdDisplay::Init() {
    vTaskDelay(pdMS_TO_TICKS(50));
    SendNibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    SendNibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    SendNibble(0x03, 0);
    SendNibble(0x02, 0);

    SendCommand(0x28);
    SendCommand(0x08);
    SendCommand(0x01);
    vTaskDelay(pdMS_TO_TICKS(2));
    SendCommand(0x06);
    SendCommand(0x0C);
    display::LoadDefaultPages(display_);
}

void LcdDisplay::StartRefreshTask(uint16_t refresh_rate_ms) {
    refresh_rate_ms_ = refresh_rate_ms;
    xTaskCreate(UpdateTask, "LCD Refresh", 2048, this, 10, nullptr);
}

void LcdDisplay::UpdateTask(void* arg) {
    auto* lcd_display = static_cast<LcdDisplay*>(arg);
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(lcd_display->refresh_rate_ms_));
        lcd_display->SendCommand(0x02);
        for (uint8_t row = 0; row < display::kRowCount; ++row) {
            for (uint8_t col = 0; col < display::kColumnCount; ++col) {
                const uint8_t value = lcd_display->display_[lcd_display->current_page_][display::kRowLayout[row]][col];
                if (value == 0) {
                    lcd_display->SendChar(0b00100000);
                } else {
                    lcd_display->SendChar(value);
                }
            }
        }
    }
}

int8_t LcdDisplay::WriteRightAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page) {
    if (data[len - 1] == 0) {
        len -= 1;
    }
    if (col - (size - 1) < 0 || len < 0 || len > size) {
        ESP_LOGI("LcdDisplay", "Data Invalid: %d", col - size);
        return -1;
    }

    uint8_t data_index = 0;
    for (int i = col - (size - 1); i <= col; i++) {
        if (i <= col - len) {
            display_[page][row][i] = 0;
        } else {
            display_[page][row][i] = data[data_index];
            data_index++;
        }
    }
    return 0;
}

int8_t LcdDisplay::WriteLeftAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page, bool sign_pad) {
    if (data[len - 1] == 0) {
        len -= 1;
        sign_pad = false;
    }

    uint8_t sign_offset = 0;
    if (sign_pad && data[0] != '-') {
        sign_offset = 1;
        display_[page][row][col] = 0;
    }

    if (col + (size - 1) > 19 || len < 0 || len > size) {
        ESP_LOGI("LcdDisplay", "Data Invalid: %d", len - col);
        return -1;
    }

    uint8_t data_index = 0;
    for (int i = 0; i < size; i++) {
        if (i >= len) {
            display_[page][row][i + col + sign_offset] = 0;
        } else {
            display_[page][row][i + col + sign_offset] = data[data_index];
            data_index++;
        }
    }
    return 0;
}

int8_t LcdDisplay::WriteDecimalAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page) {
    if (data[len - 1] == 0) {
        len -= 1;
    }
    if (col + ((size / 2) - 1) > 19 || col - (size / 2) < 0 || len < 0 || len > size) {
        ESP_LOGI("LcdDisplay", "Data Invalid: %d", len - col);
        return -1;
    }

    uint8_t dec_index = 0;
    for (int i = 0; i < col; i++) {
        if (data[i] == '.') {
            dec_index = i;
            break;
        }
        if (i == col) {
            return -1;
        }
    }

    for (int i = 0; i < (size / 2) * 2; i++) {
        display_[page][row][col - (size / 2) + i] = 0;
    }
    for (int i = 0; i < len; i++) {
        display_[page][row][(col - dec_index) + i] = data[i];
    }

    return 0;
}

void LcdDisplay::Clear() {
    for (int page = 0; page < display::kPageCount; page++) {
        for (int row = 0; row < display::kRowCount; row++) {
            for (int col = 0; col < display::kColumnCount; col++) {
                display_[page][row][col] = 0;
            }
        }
    }
}

void LcdDisplay::ToggleDisplay() {
    SetDisplayEnabled(!display_enabled_);
}

void LcdDisplay::SetDisplayEnabled(bool enabled) {
    if (display_enabled_ == enabled) {
        return;
    }

    display_enabled_ = enabled;
    if (display_enabled_) {
        SendCommand(0x08);
        backlight_ = kBacklightDisabled;
    } else {
        backlight_ = kBacklightEnabled;
        SendCommand(0x0C);
    }
}

void LcdDisplay::NextPage() {
    current_page_++;
    if (current_page_ >= display::kPageCount) {
        current_page_ = 0;
    }
    SendCommand(0x01);
}

void LcdDisplay::PrevPage() {
    current_page_--;
    if (current_page_ < 0) {
        current_page_ = display::kPageCount - 1;
    }
    SendCommand(0x01);
}

int8_t LcdDisplay::GetPage() const {
    return current_page_;
}
