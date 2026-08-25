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
#include "UrlHandles.hpp"
#include "HttpServer.hpp"


httpd_handle_t start_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &alternatorHtml);
        httpd_register_uri_handler(server, &css);
        httpd_register_uri_handler(server, &alternatorData);
        httpd_register_uri_handler(server, &alternatorReset);
    }
    return server;
}