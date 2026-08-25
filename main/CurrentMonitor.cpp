
#include <stdio.h>
#include <iostream>
#include "Gpio.hpp"
#include "driver/i2c_master.h"
#include "LcdDisplay.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"

#define SDA_PIN 15
#define SCL_PIN 23
#define PORT I2C_NUM_0
#define CLK_SRC I2C_CLK_SRC_DEFAULT
#define IGNORE_COUNT 10
#define PULLUP_ENABLED true
float lion_2_current_array[20];
// Global Variables
static i2c_master_bus_handle_t I2C_bus_handle = nullptr;

int8_t I2C_Begin()
{
    if (I2C_bus_handle == nullptr)
    {
        i2c_master_bus_config_t bus_config = {};
        bus_config.i2c_port = PORT;
        bus_config.sda_io_num = (gpio_num_t)SDA_PIN;
        bus_config.scl_io_num = (gpio_num_t)SCL_PIN;
        bus_config.clk_source = CLK_SRC;
        bus_config.glitch_ignore_cnt = IGNORE_COUNT;
        bus_config.flags.enable_internal_pullup = PULLUP_ENABLED;
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &I2C_bus_handle));
    }
    else
    {
        ESP_LOGI("I2C Controller", "Bus Already Active: %d", PORT);
        return -1;
    }
    return 0;
}

extern "C" int app_main(void)
{
    uint8_t page = 0;
    float current_buffer = 0;
    float voltage_buffer = 0;
    float temperature_buffer = 0;
    char buffer[20] = {0};
    int lion_1_offset = 369;
    int lion_2_offset = 369;
    // Analog INPUT SETUP
    // LCD SET UP
    I2C_Begin();

    LcdDisplay display(I2C_bus_handle, 0x27, 20000);
    display.lcd_init();

    display.update_lcd_task(600);
    InitGpio();

    while (1)
    {

        left_button.SetEdge();
        right_button.SetEdge();
        if ((left_button.Read() && right_button.ReadRisingEdge()) || (right_button.Read() && left_button.ReadRisingEdge()))
        {
            display.DisableDisplay();
        }
        else if (left_button.ReadRisingEdge())
        {
            //display.NextPage();
            lion_2_offset = lion_2_offset + 1;
        }
        else if (right_button.ReadRisingEdge())
        {
            //display.PrevPage();
            lion_2_offset = lion_2_offset - 1;
        }
        page = display.GetPage();
        switch (page)
        {
        case 0:
        {
            current_buffer = alternator_current.Read();
            voltage_buffer = alternator_voltage.Read();
            break;
        }
        case 1:
        {
            float lion_1_volt = 15 * lion_1_voltage.Read() / 4096.0;
            float lion_2_volt = 15 * lion_2_voltage.Read() / 4096.0;
            float lion_1_cur = lion_1_current.Read() * .173 - lion_1_offset;
            lion_2_current_array[0] = lion_2_current.Read() * .173 - lion_2_offset;
            float lion_2_cur = 0;
            printf("%d\n",lion_2_offset);
            if (1){
            for (int i = sizeof(lion_2_current_array) / sizeof(lion_2_current_array[0]) - 1; i > 0; i--)
            {
                lion_2_cur += lion_2_current_array[i];
                lion_2_current_array[i] = lion_2_current_array[i - 1];
            }
            lion_2_cur += lion_2_current_array[0];
            lion_2_cur = lion_2_cur / (sizeof(lion_2_current_array) / sizeof(lion_2_current_array[0]));
            }
            display.WriteLeftAlign(buffer, display.EncodeFloat(buffer, lion_2_volt, 1), 1, 1, 6, page);
            display.WriteLeftAlign(buffer, display.EncodeFloat(buffer, lion_2_cur, 1), 2, 1, 6, page);
            break;
        }
        case 2:
        {
            if (!diesel_heater.Read())
                display.WriteRightAlign("OFF", 3, 1, 19, 4, page);
            else
                display.WriteRightAlign("ON", 3, 1, 19, 4, page);

            if (!electric_heater.Read())
                display.WriteRightAlign("OFF", 3, 2, 19, 4, page);
            else
                display.WriteRightAlign("ON", 3, 2, 19, 4, page);

            if (!engine_heater.Read())
                display.WriteRightAlign("OFF", 3, 3, 19, 4, page);
            else
                display.WriteRightAlign("ON", 3, 3, 19, 4, page);
            break;
        }
        case 3:
        {
            if (!water_pump.Read())
                display.WriteRightAlign("OFF", 3, 1, 19, 4, page);
            else
                display.WriteRightAlign("ON", 3, 1, 19, 4, page);

            if (!cabin_pump.Read())
                display.WriteRightAlign("OFF", 3, 2, 19, 4, page);
            else
                display.WriteRightAlign("ON", 3, 2, 19, 4, page);
        }
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    return 0;
}
