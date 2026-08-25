#include "AppController.hpp"
#include "BatteryMonitorSensors.hpp"
#include "BoardConfig.hpp"
#include "I2cBus.hpp"
#include "Inputs.hpp"
#include "LcdDisplay.hpp"
#include "NetworkManager.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" int app_main(void)
{
    I2cBus i2c_bus(static_cast<gpio_num_t>(board::kI2cSdaPin),
                   static_cast<gpio_num_t>(board::kI2cSclPin),
                   board::kI2cPort);
    ESP_ERROR_CHECK(i2c_bus.Init());

    LcdDisplay lcd_display(i2c_bus.Handle(), board::kLcdAddress, board::kI2cClockHz, board::kLcdAddressLength);
    lcd_display.Init();
    lcd_display.StartRefreshTask(600);

    InitInputs();

    BatteryMonitorSensors sensors;
    network::NetworkManager network_manager;
    AppController app_controller(lcd_display, sensors, network_manager);
    app_controller.Init();

    while (1)
    {
        app_controller.Tick();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    return 0;
}
