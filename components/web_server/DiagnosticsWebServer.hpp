#pragma once

#include "BatteryMonitorSensors.hpp"
#include "esp_http_server.h"

class DiagnosticsWebServer {
public:
    explicit DiagnosticsWebServer(BatteryMonitorSensors& sensors);

    esp_err_t Start();
    httpd_handle_t Handle() const;

    static esp_err_t RootGet(httpd_req_t* req);
    static esp_err_t StyleGet(httpd_req_t* req);
    static esp_err_t ScriptGet(httpd_req_t* req);
    static esp_err_t DashboardDataGet(httpd_req_t* req);
    static esp_err_t AlternatorConfigPost(httpd_req_t* req);
    static esp_err_t BatteryConfigPost(httpd_req_t* req);
    static esp_err_t AlternatorResetPost(httpd_req_t* req);

private:
    BatteryMonitorSensors& sensors_;
    httpd_handle_t server_ = nullptr;
};
