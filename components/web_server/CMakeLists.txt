idf_component_register(
    SRCS "DiagnosticsWebServer.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_http_server sensors
)
