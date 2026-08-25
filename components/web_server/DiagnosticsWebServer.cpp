#include "DiagnosticsWebServer.hpp"

#include <cstdio>
#include "esp_check.h"
#include "esp_log.h"

namespace {

const char* kLogTag = "DiagnosticsWebServer";
const char kAlternatorHtml[] = R"HTML(<!DOCTYPE html>
<html>
    <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Alternator</title>
    <link rel="stylesheet" href="style.css">
    </head>
    <body>
        <header>
            <p>Alternator</p>
        </header>
        <table>
            <tr>
                <td><p>Max Voltage</p></td>
                <td><p id="voltageMax" class="display">0.00</p></td>
            </tr>
            <tr>
                <td><p>Voltage</p></td>
                <td><p id="voltage" class="display">0.00</p></td>
            </tr>
            <tr>
                <td><p>Max Current</p></td>
                <td><p id="currentMax" class="display">0.00</p></td>
            </tr>
            <tr>
                <td><p>Current</p></td>
                <td><p id="current" class="display">0.00</p></td>
            </tr>
        </table>
        <script src="script.js"></script>
    </body>
</html>)HTML";
const char kStyleCss[] = R"CSS(body{
    background-color: #1e1e1e;
    color: #d4d4d4;
    margin: 0px;
    align-content: center;
}
header{
    display: flex;
    background-color: #2a2a2a;
    height: 60px;
    border-radius: 10px; 
    text-align: center;
    align-items: center;
    justify-content: center;
    font-size: xx-large;
    font: bold;
}
table{
    margin: 0px;
    border-collapse: collapse;
    font-size: large;
    align-content: center;
    width: 100%;
}
table td{
    margin: 0px;
    text-align: center;
    width:50%;
    border-top: #d4d4d4 solid 1px;
    border-bottom: #d4d4d4 solid 1px;
}
.display{
    width: 90%;
    height: 100%;
    font-size: 35px;
    background-color: #4a4a4a;
    border-radius: 10px;
    text-align: center;
})CSS";
const char kScriptJs[] = R"JS(function fetchAlternatorData() {
        fetch("/api/alternator")
            .then(res => res.json())
            .then(data => {
                document.getElementById("current").textContent = data.current;
                document.getElementById("currentMax").textContent = data.currentMax;
                document.getElementById("voltage").textContent = data.voltage;
                document.getElementById("voltageMax").textContent = data.voltageMax;
            })
        .catch(err => {console.error("Error fetching data:", err)
                document.getElementById("current").textContent = "Error";
                document.getElementById("currentMax").textContent = "Error";
                document.getElementById("voltage").textContent = "Error";
                document.getElementById("voltageMax").textContent = "Error";
        });
}

fetchAlternatorData();
setInterval(fetchAlternatorData, 500);
)JS";

httpd_uri_t RootUri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = DiagnosticsWebServer::RootGet,
    .user_ctx = nullptr,
};

httpd_uri_t StyleUri = {
    .uri = "/style.css",
    .method = HTTP_GET,
    .handler = DiagnosticsWebServer::StyleGet,
    .user_ctx = nullptr,
};

httpd_uri_t ScriptUri = {
    .uri = "/script.js",
    .method = HTTP_GET,
    .handler = DiagnosticsWebServer::ScriptGet,
    .user_ctx = nullptr,
};

httpd_uri_t AlternatorDataUri = {
    .uri = "/api/alternator",
    .method = HTTP_GET,
    .handler = DiagnosticsWebServer::AlternatorDataGet,
    .user_ctx = nullptr,
};

httpd_uri_t AlternatorResetUri = {
    .uri = "/api/alternator/reset",
    .method = HTTP_POST,
    .handler = DiagnosticsWebServer::AlternatorResetPost,
    .user_ctx = nullptr,
};

}  // namespace

DiagnosticsWebServer::DiagnosticsWebServer(BatteryMonitorSensors& sensors)
    : sensors_(sensors) {
    AlternatorDataUri.user_ctx = this;
    AlternatorResetUri.user_ctx = this;
}

esp_err_t DiagnosticsWebServer::Start() {
    if (server_ != nullptr) {
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_RETURN_ON_ERROR(httpd_start(&server_, &config), kLogTag, "Failed to start HTTP server");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &RootUri), kLogTag, "Failed to register root route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &StyleUri), kLogTag, "Failed to register style route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &ScriptUri), kLogTag, "Failed to register script route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &AlternatorDataUri), kLogTag, "Failed to register data route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &AlternatorResetUri), kLogTag, "Failed to register reset route");
    return ESP_OK;
}

httpd_handle_t DiagnosticsWebServer::Handle() const {
    return server_;
}

esp_err_t DiagnosticsWebServer::RootGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, kAlternatorHtml, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::StyleGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, kStyleCss, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::ScriptGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "application/javascript");
    return httpd_resp_send(req, kScriptJs, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::AlternatorDataGet(httpd_req_t* req) {
    auto* server = static_cast<DiagnosticsWebServer*>(req->user_ctx);
    const BatteryReadings& readings = server->sensors_.LatestReadings();
    char response[256] = {0};
    std::snprintf(response,
                  sizeof(response),
                  "{\"currentMax\": %.2f, \"voltageMax\": %.2f, \"current\": %.2f, \"voltage\": %.2f}",
                  readings.alternator_current_max,
                  readings.alternator_voltage_max,
                  readings.alternator_current_raw,
                  readings.alternator_voltage_raw);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::AlternatorResetPost(httpd_req_t* req) {
    auto* server = static_cast<DiagnosticsWebServer*>(req->user_ctx);
    server->sensors_.ResetAlternatorPeaks();
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
}
