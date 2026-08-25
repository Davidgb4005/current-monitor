idf_component_register(
    SRCS "main.cpp"
    REQUIRES nvs_flash esp_netif esp_wifi esp_http_server esp_event spiffs http_server setup Gpio PinController
    INCLUDE_DIRS "."
)
