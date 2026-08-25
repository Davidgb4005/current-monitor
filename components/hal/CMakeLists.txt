idf_component_register(
    SRCS
        "I2cBus.cpp"
        "PinController.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c esp_driver_gpio esp_event esp_adc esp_driver_ledc esp_driver_uart esp_timer log
)
