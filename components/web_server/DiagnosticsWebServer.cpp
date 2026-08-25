#include "DiagnosticsWebServer.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Inputs.hpp"
#include "esp_check.h"
#include "esp_log.h"

namespace {

const char* kLogTag = "DiagnosticsWebServer";
const char kOkResponse[] = "{\"status\":\"ok\"}";
const char kDashboardHtml[] = R"HTML(<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Diagnostics</title>
<link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <p id="pageTitle">Alternator</p>
    </header>
    <nav class="tabs">
        <button class="tab active" data-page="alternator">Alternator</button>
        <button class="tab" data-page="battery">Battery</button>
        <button class="tab" data-page="heaters">Heaters</button>
        <button class="tab" data-page="pumps">Pumps</button>
    </nav>

    <main>
        <section class="page active" id="page-alternator">
            <table>
                <tr><td><p>Max Voltage</p></td><td><p id="voltageMax" class="display">0.00</p></td></tr>
                <tr><td><p>Voltage</p></td><td><p id="voltage" class="display">0.00</p></td></tr>
                <tr><td><p>Max Current</p></td><td><p id="currentMax" class="display">0.00</p></td></tr>
                <tr><td><p>Current</p></td><td><p id="current" class="display">0.00</p></td></tr>
                <tr><td><p>Energy (kJ)</p></td><td><p id="energyJoules" class="display">0.00</p></td></tr>
            </table>
            <div class="controls">
                <label class="controlRow"><span>Current Offset</span><input id="currentOffsetInput" type="number" step="0.01"></label>
                <label class="controlRow"><span>Current Slope</span><input id="currentSlopeInput" type="number" step="0.001"></label>
                <label class="controlRow"><span>Voltage Offset</span><input id="voltageOffsetInput" type="number" step="0.01"></label>
                <label class="controlRow"><span>Voltage Slope</span><input id="voltageSlopeInput" type="number" step="0.0001"></label>
                <div class="buttonRow">
                    <button id="saveCalButton">Save Calibration</button>
                    <button id="resetMaxButton">Reset</button>
                </div>
            </div>
        </section>

        <section class="page" id="page-battery">
            <table>
                <tr><td><p>Passenger Voltage</p></td><td><p id="passengerVoltage" class="display">0.00</p></td></tr>
                <tr><td><p>Passenger Current</p></td><td><p id="passengerCurrent" class="display">0.00</p></td></tr>
                <tr><td><p>Driver Voltage</p></td><td><p id="driverVoltage" class="display">0.00</p></td></tr>
                <tr><td><p>Driver Current</p></td><td><p id="driverCurrent" class="display">0.00</p></td></tr>
            </table>
        </section>

        <section class="page" id="page-heaters">
            <table>
                <tr><td><p>Diesel Heater</p></td><td><p id="dieselHeater" class="display state">OFF</p></td></tr>
                <tr><td><p>Electric Heater</p></td><td><p id="electricHeater" class="display state">OFF</p></td></tr>
                <tr><td><p>Engine Heater</p></td><td><p id="engineHeater" class="display state">OFF</p></td></tr>
            </table>
        </section>

        <section class="page" id="page-pumps">
            <table>
                <tr><td><p>Water Pump</p></td><td><p id="waterPump" class="display state">OFF</p></td></tr>
                <tr><td><p>Cabin Pump</p></td><td><p id="cabinPump" class="display state">OFF</p></td></tr>
            </table>
        </section>
    </main>

    <script src="/script.js"></script>
</body>
</html>)HTML";
const char kStyleCss[] = R"CSS(body{
    background-color: #1e1e1e;
    color: #d4d4d4;
    margin: 0px;
    align-content: center;
    font-family: sans-serif;
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
    margin: 8px;
}
.tabs{
    display: grid;
    grid-template-columns: repeat(4, 1fr);
    gap: 8px;
    margin: 8px;
}
.tab,
.buttonRow button{
    background-color: #2a2a2a;
    color: #d4d4d4;
    border: 1px solid #4a4a4a;
    border-radius: 10px;
    padding: 12px 8px;
    font-size: 16px;
}
.tab.active{
    background-color: #4a4a4a;
}
.page{
    display: none;
}
.page.active{
    display: block;
}
table{
    margin: 8px 0;
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
    margin: 8px auto;
    padding: 8px 0;
}
.controls{
    margin: 12px 8px;
    display: grid;
    gap: 10px;
}
.controlRow{
    display: grid;
    grid-template-columns: 120px 1fr;
    align-items: center;
    gap: 12px;
}
.controlRow input{
    background-color: #4a4a4a;
    color: #d4d4d4;
    border: 1px solid #666;
    border-radius: 10px;
    padding: 10px;
    font-size: 18px;
}
.buttonRow{
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 8px;
}
.state{
    font-size: 28px;
}
@media (max-width: 600px){
    .tabs{
        grid-template-columns: repeat(2, 1fr);
    }
}
)CSS";
const char kScriptJs[] = R"JS(const pageTitle = document.getElementById("pageTitle");
const tabs = [...document.querySelectorAll(".tab")];
const pages = [...document.querySelectorAll(".page")];

function showPage(name) {
    tabs.forEach(tab => tab.classList.toggle("active", tab.dataset.page === name));
    pages.forEach(page => page.classList.toggle("active", page.id === `page-${name}`));
    pageTitle.textContent = name.charAt(0).toUpperCase() + name.slice(1);
}

tabs.forEach(tab => {
    tab.addEventListener("click", () => showPage(tab.dataset.page));
});

function setText(id, value) {
    const node = document.getElementById(id);
    if (node) {
        node.textContent = value;
    }
}

function setIfNotFocused(id, value) {
    const node = document.getElementById(id);
    if (node && document.activeElement !== node) {
        node.value = value;
    }
}

function stateLabel(value) {
    return value ? "ON" : "OFF";
}

async function fetchDashboard() {
    try {
        const res = await fetch("/api/dashboard");
        const data = await res.json();
        setText("current", data.alternator.current.toFixed(2));
        setText("currentMax", data.alternator.currentMax.toFixed(2));
        setText("voltage", data.alternator.voltage.toFixed(2));
        setText("voltageMax", data.alternator.voltageMax.toFixed(2));
        setText("energyJoules", data.alternator.energyKilojoules.toFixed(2));
        setIfNotFocused("currentOffsetInput", data.alternator.currentOffset.toFixed(2));
        setIfNotFocused("currentSlopeInput", data.alternator.currentSlope.toFixed(3));
        setIfNotFocused("voltageOffsetInput", data.alternator.voltageOffset.toFixed(2));
        setIfNotFocused("voltageSlopeInput", data.alternator.voltageSlope.toFixed(4));

        setText("passengerVoltage", data.battery.passengerVoltage.toFixed(2));
        setText("passengerCurrent", data.battery.passengerCurrent.toFixed(2));
        setText("driverVoltage", data.battery.driverVoltage.toFixed(2));
        setText("driverCurrent", data.battery.driverCurrent.toFixed(2));

        setText("dieselHeater", stateLabel(data.heaters.diesel));
        setText("electricHeater", stateLabel(data.heaters.electric));
        setText("engineHeater", stateLabel(data.heaters.engine));

        setText("waterPump", stateLabel(data.pumps.water));
        setText("cabinPump", stateLabel(data.pumps.cabin));
    } catch (err) {
        console.error("Error fetching dashboard:", err);
    }
}

async function saveCalibration() {
    const currentOffset = document.getElementById("currentOffsetInput").value;
    const currentSlope = document.getElementById("currentSlopeInput").value;
    const voltageOffset = document.getElementById("voltageOffsetInput").value;
    const voltageSlope = document.getElementById("voltageSlopeInput").value;
    await fetch("/api/alternator/config", {
        method: "POST",
        headers: {"Content-Type": "application/x-www-form-urlencoded"},
        body: `currentOffset=${encodeURIComponent(currentOffset)}&currentSlope=${encodeURIComponent(currentSlope)}&voltageOffset=${encodeURIComponent(voltageOffset)}&voltageSlope=${encodeURIComponent(voltageSlope)}`
    });
    fetchDashboard();
}

async function resetMax() {
    await fetch("/api/alternator/reset", {method: "POST"});
    fetchDashboard();
}

document.getElementById("saveCalButton").addEventListener("click", saveCalibration);
document.getElementById("resetMaxButton").addEventListener("click", resetMax);

showPage("alternator");
fetchDashboard();
setInterval(fetchDashboard, 500);
)JS";

void CopyFormValue(char* destination, size_t destination_size, const char* body, const char* key) {
    if (destination_size == 0) {
        return;
    }
    destination[0] = '\0';
    const char* start = std::strstr(body, key);
    if (start == nullptr) {
        return;
    }
    start += std::strlen(key);
    const char* end = std::strchr(start, '&');
    size_t len = end == nullptr ? std::strlen(start) : static_cast<size_t>(end - start);
    if (len >= destination_size) {
        len = destination_size - 1;
    }
    std::memcpy(destination, start, len);
    destination[len] = '\0';
}

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

httpd_uri_t DashboardUri = {
    .uri = "/api/dashboard",
    .method = HTTP_GET,
    .handler = DiagnosticsWebServer::DashboardDataGet,
    .user_ctx = nullptr,
};

httpd_uri_t AlternatorConfigUri = {
    .uri = "/api/alternator/config",
    .method = HTTP_POST,
    .handler = DiagnosticsWebServer::AlternatorConfigPost,
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
    DashboardUri.user_ctx = this;
    AlternatorConfigUri.user_ctx = this;
    AlternatorResetUri.user_ctx = this;
}

esp_err_t DiagnosticsWebServer::Start() {
    if (server_ != nullptr) {
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_RETURN_ON_ERROR(httpd_start(&server_, &config), kLogTag, "Failed to start HTTP server");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &RootUri), kLogTag, "Failed to register root route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &StyleUri), kLogTag, "Failed to register style route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &ScriptUri), kLogTag, "Failed to register script route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &DashboardUri), kLogTag, "Failed to register dashboard route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &AlternatorConfigUri), kLogTag, "Failed to register config route");
    ESP_RETURN_ON_ERROR(httpd_register_uri_handler(server_, &AlternatorResetUri), kLogTag, "Failed to register reset route");
    return ESP_OK;
}

httpd_handle_t DiagnosticsWebServer::Handle() const {
    return server_;
}

esp_err_t DiagnosticsWebServer::RootGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, kDashboardHtml, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::StyleGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, kStyleCss, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::ScriptGet(httpd_req_t* req) {
    httpd_resp_set_type(req, "application/javascript");
    return httpd_resp_send(req, kScriptJs, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::DashboardDataGet(httpd_req_t* req) {
    auto* server = static_cast<DiagnosticsWebServer*>(req->user_ctx);
    const BatteryReadings& readings = server->sensors_.LatestReadings();
    char response[768] = {0};
    std::snprintf(
        response,
        sizeof(response),
        "{"
        "\"alternator\":{\"current\":%.2f,\"currentMax\":%.2f,\"voltage\":%.2f,\"voltageMax\":%.2f,\"currentOffset\":%.2f,\"currentSlope\":%.3f,\"voltageOffset\":%.2f,\"voltageSlope\":%.4f,\"energyKilojoules\":%.3f},"
        "\"battery\":{\"passengerVoltage\":%.2f,\"passengerCurrent\":%.2f,\"driverVoltage\":%.2f,\"driverCurrent\":%.2f},"
        "\"heaters\":{\"diesel\":%s,\"electric\":%s,\"engine\":%s},"
        "\"pumps\":{\"water\":%s,\"cabin\":%s}"
        "}",
        readings.alternator_current,
        readings.alternator_current_max,
        readings.alternator_voltage,
        readings.alternator_voltage_max,
        readings.alternator_current_offset,
        readings.alternator_current_slope,
        readings.alternator_voltage_offset,
        readings.alternator_voltage_slope,
        readings.alternator_energy_joules / 1000.0f,
        readings.passenger_voltage,
        readings.passenger_current,
        readings.driver_voltage,
        readings.driver_current,
        diesel_heater.Read() ? "true" : "false",
        electric_heater.Read() ? "true" : "false",
        engine_heater.Read() ? "true" : "false",
        water_pump.Read() ? "true" : "false",
        cabin_pump.Read() ? "true" : "false");
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::AlternatorConfigPost(httpd_req_t* req) {
    auto* server = static_cast<DiagnosticsWebServer*>(req->user_ctx);
    char body[128] = {0};
    int received = httpd_req_recv(req, body, std::min<int>(req->content_len, static_cast<int>(sizeof(body) - 1)));
    if (received <= 0) {
        return ESP_FAIL;
    }
    body[received] = '\0';

    char current_offset_buffer[32] = {0};
    char current_slope_buffer[32] = {0};
    char voltage_offset_buffer[32] = {0};
    char voltage_slope_buffer[32] = {0};
    CopyFormValue(current_offset_buffer, sizeof(current_offset_buffer), body, "currentOffset=");
    CopyFormValue(current_slope_buffer, sizeof(current_slope_buffer), body, "currentSlope=");
    CopyFormValue(voltage_offset_buffer, sizeof(voltage_offset_buffer), body, "voltageOffset=");
    CopyFormValue(voltage_slope_buffer, sizeof(voltage_slope_buffer), body, "voltageSlope=");

    if (current_offset_buffer[0] != '\0') {
        server->sensors_.SetAlternatorCurrentOffset(std::strtof(current_offset_buffer, nullptr));
    }
    if (current_slope_buffer[0] != '\0') {
        server->sensors_.SetAlternatorCurrentSlope(std::strtof(current_slope_buffer, nullptr));
    }
    if (voltage_offset_buffer[0] != '\0') {
        server->sensors_.SetAlternatorVoltageOffset(std::strtof(voltage_offset_buffer, nullptr));
    }
    if (voltage_slope_buffer[0] != '\0') {
        server->sensors_.SetAlternatorVoltageSlope(std::strtof(voltage_slope_buffer, nullptr));
    }

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, kOkResponse, HTTPD_RESP_USE_STRLEN);
}

esp_err_t DiagnosticsWebServer::AlternatorResetPost(httpd_req_t* req) {
    auto* server = static_cast<DiagnosticsWebServer*>(req->user_ctx);
    server->sensors_.ResetAlternatorPeaks();
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, kOkResponse, HTTPD_RESP_USE_STRLEN);
}
