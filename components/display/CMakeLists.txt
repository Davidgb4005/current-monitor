idf_component_register(
    SRCS
        "DisplayFormatter.cpp"
        "DisplayPages.cpp"
        "LcdDisplay.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c freertos log
)
