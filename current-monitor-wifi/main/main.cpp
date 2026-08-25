#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_spiffs.h"
#include <cstring> 
#include "../components/setup/Setup.hpp"
#include "../components/http_server/HttpServer.hpp"
#include "../components/http_server/UrlHandles.hpp"
#include "../components/Gpio/Gpio.hpp"
#include "../components/PinController/PinController.hpp"
#define AP_SSID "ESP32_AP"
#define AP_PASS "12345678"
#define MAX_STA_CONN 4

static const char *TAG = "HTTP_SERVER";



extern "C" void app_main(void)
{
    char buffer[1024];
    const char * my_str = "{\"currentMax\": 111, \"voltageMax\": 13.6,\"current\": 100, \"voltage\": 12.5}";
    const char * my_str2 = "{\"currentMax\": 0, \"voltageMax\": 0,\"current\": 100, \"voltage\": 12.5}";
    sent_msg_ptr = my_str;
    setup();
    start_server();
    int i = 0;
    InitGpio();
    static float alternator_current = 0;
    static float alternator_voltage = 0;
    while(1){
        float alternator_volt = 15 * lion_2_voltage.Read() / 4096.0;
        float lion_2_volt = 15 * alternator_current.Read() / 4096.0;

        alternator_current_array[0] = lion_2_current.Read() * .173 - lion_2_offset;
        float alternator_cur = 0;
        printf("%d\n",alternator_offset);
        if (1){
        for (int i = sizeof(alternator_current_array) / sizeof(alternator_current_array[0]) - 1; i > 0; i--)
        {
            alternator_cur += alternator_current_array[i];
            alternator_current_array[i] = alternator_current_array[i - 1];
        }
        alternator_cur += alternator_current_array[0];
        alternator_cur = alternator_cur / (sizeof(alternator_current_array) / sizeof(alternator_current_array[0]));
        }
        if (alternator_volt > alternator_volt_max){
            alternator_volt_max = alternator_volt;
        }
        if (alternator_current > alternator_current_max){
            alternator_current_max = alternator_current;
        }
        sprintf(buffer, "{\"currentMax\": %f, \"voltageMax\": %f,\"current\": %f, \"voltage\": %f}",alternator_current_max,alternator_volt,alternator_current,alternator_volt);
        if (*recv_msg_ptr == 'r'){
            alternator_volt_max = 0;
            alternator_current_max = 0;
            *recv_msg_ptr = '\0';
            i=0;
        }
        sent_msg_ptr = buffer;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
