idf_component_register(SRCS "CurrentMonitor.cpp"
                    REQUIRES "app_core" "display" "board_hal" "input" "network" "sensors" "web_server"
                    INCLUDE_DIRS ".")
