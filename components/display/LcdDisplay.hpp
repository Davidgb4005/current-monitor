#pragma once

#include <cstdint>

#include "DisplayPages.hpp"
#include "driver/i2c_master.h"

class LcdDisplay {
public:
    LcdDisplay(i2c_master_bus_handle_t bus_handle,
               uint8_t address,
               uint16_t frequency,
               i2c_addr_bit_len_t address_length = I2C_ADDR_BIT_LEN_7);
    ~LcdDisplay() = default;

    void Init();
    void Clear();
    int8_t WriteRightAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page);
    int8_t WriteLeftAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page, bool sign_pad = true);
    int8_t WriteDecimalAlign(const char* data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page);
    void StartRefreshTask(uint16_t refresh_rate_ms);

    void ToggleDisplay();
    int8_t GetPage() const;
    void NextPage();
    void PrevPage();

private:
    static constexpr uint8_t kBacklightEnabled = 0x08;
    static constexpr uint8_t kBacklightDisabled = 0x00;

    esp_err_t WriteByte(uint8_t data);
    void ToggleEnable(uint8_t data);
    void SendNibble(uint8_t nibble, uint8_t rs);
    void SendByte(uint8_t byte, uint8_t rs);
    void SendCommand(uint8_t cmd);
    void SendChar(char c);
    static void UpdateTask(void* arg);

    i2c_master_dev_handle_t dev_handle_ = nullptr;
    uint8_t display_[display::kPageCount][display::kRowCount][display::kColumnCount + 1] = {};
    int8_t current_page_ = 1;
    uint16_t refresh_rate_ms_ = 10000;
    bool display_enabled_ = true;
    uint8_t backlight_ = kBacklightEnabled;
};
