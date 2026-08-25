
#include "LcdDisplay.hpp"
#define I2C_MASTER_TIMEOUT_MS 1000
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE 0x04
#define LCD_READ_WRITE 0x02
#define LCD_REGISTER_SELECT 0x01



LcdDisplay::LcdDisplay(i2c_master_bus_handle_t bus_handle, uint8_t adr, uint16_t freq, i2c_addr_bit_len_t adr_len)
{
    i2c_device_config_t dev_config = {};
    dev_config.dev_addr_length = adr_len;
    dev_config.device_address = adr; // dummy address
    dev_config.scl_speed_hz = freq;

    esp_err_t err = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI("I2C", "Failed To Add Device: %d", adr);
    }
}

LcdDisplay::~LcdDisplay()
{
}
esp_err_t LcdDisplay::lcd_write_byte(uint8_t data)
{
    uint8_t payload = data | backlight;
    esp_err_t ret = i2c_master_transmit(dev_handle, &payload, 1, I2C_MASTER_TIMEOUT_MS);
    return ret;
}

void LcdDisplay::lcd_toggle_enable(uint8_t data)
{
    lcd_write_byte(data | LCD_ENABLE);
    lcd_write_byte(data & ~LCD_ENABLE);
}

void LcdDisplay::lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble << 4) | backlight;
    if (rs)
        data |= LCD_REGISTER_SELECT;
    lcd_toggle_enable(data);
}

void LcdDisplay::lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_send_nibble(byte >> 4, rs);   // high nibble
    lcd_send_nibble(byte & 0x0F, rs); // low nibble
}

void LcdDisplay::lcd_send_command(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);
}

void LcdDisplay::lcd_send_char(char c)
{
    lcd_send_byte(c, 1);
}

void LcdDisplay::lcd_init()
{
    vTaskDelay(pdMS_TO_TICKS(50));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_nibble(0x03, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_nibble(0x03, 0);
    lcd_send_nibble(0x02, 0); // 4-bit mode

    lcd_send_command(0x28); // Function set: 4-bit, 2 line, 5x8 dots
    lcd_send_command(0x08); // Display off
    lcd_send_command(0x01); // Clear display
    vTaskDelay(pdMS_TO_TICKS(2));
    lcd_send_command(0x06); // Entry mode
    lcd_send_command(0x0C); // Display on, cursor off
    for (int k = 0;k<PAGES;k++){
        for (size_t j = 0; j < ROWS; j++)
        {
            for (size_t i = 0; i < COLS; i++)
            {
                this->display[k][j][i] = default_display[k][j][i];
            }
            
        }
        
    }
}
void LcdDisplay::update_lcd_task(uint16_t refresh_rate)
{
    this->refresh_rate = refresh_rate;
    xTaskCreate(
        update_lcd,    // Task function
        "LCD Refresh", // Name (for debugging)
        2048,          // Stack size in words
        this,          // Task parameters
        10,            // Priority (higher = more urgent)
        nullptr        // Task handle (optional)
    );
}
void LcdDisplay::update_lcd(void *arg)
{
    LcdDisplay *ldc_display = (LcdDisplay *)arg;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(ldc_display->refresh_rate));
        ldc_display->lcd_send_command(0x02);
        for (int k = 0; k < sizeof(ROW_LAYOUT); k++)
        {
            for (int i = 0; i < 20; i++)
            {
                if ((ldc_display->display[ldc_display->current_page][ROW_LAYOUT[k]][i]) == 0)
                {
                    ldc_display->lcd_send_char(0b00100000); // 0 sets the position to clear
                }
                else
                {
                    ldc_display->lcd_send_char(ldc_display->display[ldc_display->current_page][ROW_LAYOUT[k]][i]);
                }
            }
        }
    }
}

int8_t LcdDisplay::WriteRightAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page)
{
    if (data[len - 1] == 0)
    {
        len -= 1;
    }
    if (col - (size - 1) < 0 || len < 0 || len > size) // Enusure Data Will Fit
    {
        ESP_LOGI("Lcd Controller", "Data Invalid: %d", col - size);

        return -1;
    }
    else
    {
        uint8_t k = 0;
        for (int i = col - (size - 1); i <= col; i++)
        {

            if (i <= col - len)
            {
                display[page][row][i] = 0;
            }
            else
            {
                display[page][row][i] = data[k];
                k++;
            }
        }
        return 0;
    }
}

int8_t LcdDisplay::WriteLeftAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page, bool sign_pad)
{
    if (data[len - 1] == 0) // Remove Null Termination Char from Strings
    {
        len -= 1;
        sign_pad = false; // there is never padding needed for strings
    }
    uint8_t sign_offset = 0;
    if (sign_pad && data[0] != '-') // Pad First Spot with 0
    {
        sign_offset = 1;
        display[page][row][col] = 0;
    }

    if (col + (size - 1) > 19 || len < 0 || len > size) // Enusure Data Will Fit
    {
        ESP_LOGI("Lcd Controller", "Data Invalid: %d", len - col);
        return -1;
    }
    else
    {
        uint8_t k = 0;
        for (int i = 0; i < size; i++)
        {
            if (i >= len)
            {
                display[page][row][i + col + sign_offset] = 0;
            }
            else
            {
                display[page][row][i + col + sign_offset] = data[k];
                k++;
            }
        }
        return 0;
    }
}

int8_t LcdDisplay::WriteDecimalAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page)
{
    if (data[len - 1] == 0)
    {
        len -= 1;
    }
    if (col + ((size / 2) - 1) > 19 || col - (size / 2) < 0 || len < 0 || len > size) // Enusure Data Will Fit
    {
        ESP_LOGI("Lcd Controller", "Data Invalid: %d", len - col);
        return -1;
    }
    else
    {
        uint8_t dec_index = 0;
        for (int i = 0; i < col; i++)
        {
            if (data[i] == '.')
            {
                dec_index = i;
                ESP_LOGI("Lcd Controller", "Dec Index: %d", dec_index);
                break;
            }
            if (i == col)
            {
                return -1;
            }
        }

        for (int i = 0; i < (size / 2) * 2; i++)
        {
            display[page][row][col - (size / 2) + i] = 0;
        }
        for (int i = 0; i < len; i++)
        {

            display[page][row][(col - dec_index) + i] = data[i];
        }
    }
    return 0;
}
i2c_master_dev_handle_t *LcdDisplay::GetDevHandle()
{
    return &dev_handle;
}

void LcdDisplay::LcdClear()
{
    for (int k = 0; k < max_page; k++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 20; i++)
            {
                display[k][j][i] = 0;
            }
        }
    }
}

int8_t LcdDisplay::EncodeInt(char *buffer, int32_t value)
{
    uint8_t i = 0;
    int8_t is_neg = 0;
    if (value < 0)
    {
        is_neg = 1;
        buffer[0] = '-';
        value = value * -1;
    }
    else if (value == 0)
    {
        buffer[0] = '0';
        i++;
        return i;
    }

    while (value > 0)
    {
        buffer[20 - i] = '0' + (value % 10);
        value = value / 10;
        i++;
        if (i > 20)
        {
            ESP_LOGI("Lcd Controller", "Conversion Failure: %d", value);
            return -1;
        }
    }

    if (i < 20)
    {
        int k;
        for (k = 0; k <= i; k++)
        {
            buffer[k + is_neg] = buffer[20 - i + 1 + k];
        }
    }
    return i + is_neg;
}

int8_t LcdDisplay::EncodeFloat(char *buffer, float value, int positions)
{
    uint8_t temp_buffer[20] = {0};
    uint8_t i = 0;
    int8_t is_neg = 0;
    if (value < 0)
    {
        is_neg = 1;
        buffer[0] = '-';
        value = value * -1;
    }

    int32_t exponent = 10;
    for (int k = 0; k < positions - 1; k++)
    {
        exponent *= 10;
    }
    int32_t remainder = value * exponent;
    int32_t quotient = value;

    //--------------
    for (int k = 0; k < positions; k++)
    {
        temp_buffer[20 - i] = '0' + (remainder % 10);
        remainder = remainder / 10;
        i++;

        if (i > 20)
        {
            ESP_LOGI("Lcd Controller", "Conversion Failure: %d", value);
            return -1;
        }
    }

    temp_buffer[20 - i] = '.';
    i++;
    if (quotient == 0)
    {
        temp_buffer[20 - i] = '0';
        i++;
    }
    while (quotient > 0)
    {

        temp_buffer[20 - i] = '0' + (quotient % 10);
        quotient = quotient / 10;
        i++;
        if (i > 20)
        {
            ESP_LOGI("Lcd Controller", "Conversion Failure: %d", value);
            return -1;
        }
    }

    if (i < 20)
    {
        for (int k = 0; k <= i; k++)
        {
            buffer[k + is_neg] = temp_buffer[20 - (i - 1) + k];
        }
    }
    return i + is_neg;
}

void LcdDisplay::DisableDisplay()
{
    display_enabled = !display_enabled;
    if (display_enabled)
    {
        lcd_send_command(0x08);
        backlight = 0x00;
    }
    else
    {
        backlight = 0x08;
        lcd_send_command(0x0C);
    }
}

void LcdDisplay::NextPage()
{
    current_page++;
    if (current_page >= max_page)
    {
        current_page = 0;
    }
    lcd_send_command(0x01);
}
void LcdDisplay::PrevPage()
{
    current_page--;
    if (current_page < 0)
    {
        current_page = max_page - 1;
    }
    lcd_send_command(0x01);
}
int8_t LcdDisplay::GetPage(){
    return current_page;
}