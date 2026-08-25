idf_component_register(
    SRCS "NetworkManager.cpp"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash esp_netif esp_wifi esp_event
)
