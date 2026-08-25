idf_component_register(SRCS "CurrentMonitor.cpp"
                    REQUIRES "esp_driver_i2c" "PinController" "LcdDisplay" "Gpio"
                    INCLUDE_DIRS ".")
