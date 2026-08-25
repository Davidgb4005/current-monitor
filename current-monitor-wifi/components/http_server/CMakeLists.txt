idf_component_register(SRCS "HttpServer.cpp" "UrlHandlerFunctions.cpp" "UrlHandles.cpp"
                    REQUIRES "nvs_flash" "esp_netif" "esp_wifi" "esp_http_server" " esp_event" "spiffs"
                    INCLUDE_DIRS ".")