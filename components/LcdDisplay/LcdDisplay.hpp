#include <stdint.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "LcdDisplayConfig.cpp"


class LcdDisplay
{
private:
    i2c_master_dev_handle_t dev_handle;
    uint8_t max_page = PAGES;

    uint8_t display[PAGES][ROWS][COLS+1] = {};
    int8_t current_page = 1;
    esp_err_t lcd_write_byte(uint8_t data);
    void lcd_toggle_enable(uint8_t data);
    void lcd_send_nibble(uint8_t nibble, uint8_t rs);
    void lcd_send_byte(uint8_t byte, uint8_t rs);
    void lcd_send_command(uint8_t cmd);
    void lcd_send_char(char c);
    uint16_t refresh_rate = 10000;
    static void update_lcd(void *arg);
    bool display_enabled = true;
    uint8_t backlight = 0x08;

public:
    LcdDisplay(i2c_master_bus_handle_t bus_handle, uint8_t adr, uint16_t freq, i2c_addr_bit_len_t adr_len = I2C_ADDR_BIT_LEN_7);
    ~LcdDisplay();
    void lcd_init();
    void LcdClear();
    int8_t WriteRightAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page);
    int8_t WriteLeftAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page, bool sign_pad = true);
    i2c_master_dev_handle_t *GetDevHandle();
    int8_t WriteDecimalAlign(const char *data, int8_t len, uint8_t row, uint8_t col, uint8_t size, uint8_t page);
    int8_t EncodeFloat(char *buffer, float value, int positions);
    int8_t EncodeInt(char *buffer, int32_t value);
    void update_lcd_task(uint16_t refresh_rate);

    void DisableDisplay();
    int8_t GetPage();
    void NextPage();
    void PrevPage();
};
