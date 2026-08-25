idf_component_register(
    SRCS "LcdDisplayConfig.cpp" "LcdDisplay.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_event esp_driver_i2c log
)
