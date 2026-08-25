#include "BatteryMonitorSensors.hpp"

#include "Inputs.hpp"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"

namespace {

const char* kLogTag = "BatteryMonitorSensors";
const char* kCalibrationNamespace = "calib";

constexpr const char* kPassengerCurrentOffsetKey = "pas_i_off";
constexpr const char* kPassengerCurrentSlopeKey = "pas_i_slp";
constexpr const char* kPassengerVoltageOffsetKey = "pas_v_off";
constexpr const char* kPassengerVoltageSlopeKey = "pas_v_slp";
constexpr const char* kDriverCurrentOffsetKey = "drv_i_off";
constexpr const char* kDriverCurrentSlopeKey = "drv_i_slp";
constexpr const char* kDriverVoltageOffsetKey = "drv_v_off";
constexpr const char* kDriverVoltageSlopeKey = "drv_v_slp";
constexpr const char* kAlternatorCurrentOffsetKey = "alt_i_off";
constexpr const char* kAlternatorCurrentSlopeKey = "alt_i_slp";
constexpr const char* kAlternatorVoltageOffsetKey = "alt_v_off";
constexpr const char* kAlternatorVoltageSlopeKey = "alt_v_slp";

esp_err_t InitializeNvsFlash() {
    esp_err_t result = nvs_flash_init();
    if (result == ESP_ERR_NVS_NO_FREE_PAGES || result == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        result = nvs_flash_init();
    }
    return result;
}

void LoadFloatIfPresent(nvs_handle_t handle, const char* key, float& value) {
    size_t required_size = sizeof(value);
    float loaded = value;
    esp_err_t result = nvs_get_blob(handle, key, &loaded, &required_size);
    if (result == ESP_OK && required_size == sizeof(loaded)) {
        value = loaded;
    }
}

void SaveFloat(nvs_handle_t handle, const char* key, float value) {
    ESP_ERROR_CHECK(nvs_set_blob(handle, key, &value, sizeof(value)));
}

}  // namespace

esp_err_t BatteryMonitorSensors::Init() {
    if (storage_ready_) {
        return ESP_OK;
    }

    esp_err_t result = InitializeNvsFlash();
    if (result != ESP_OK) {
        ESP_LOGE(kLogTag, "Failed to initialize NVS: %s", esp_err_to_name(result));
        return result;
    }

    result = LoadCalibration();
    if (result != ESP_OK) {
        ESP_LOGE(kLogTag, "Failed to load calibration: %s", esp_err_to_name(result));
        return result;
    }

    storage_ready_ = true;
    return ESP_OK;
}

void BatteryMonitorSensors::SetPassengerCurrentOffset(float offset) {
    passenger_current_offset_ = offset;
    latest_readings_.passenger_current_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetPassengerCurrentSlope(float slope) {
    passenger_current_slope_ = slope;
    latest_readings_.passenger_current_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetPassengerCurrentOffset() const {
    return passenger_current_offset_;
}

float BatteryMonitorSensors::GetPassengerCurrentSlope() const {
    return passenger_current_slope_;
}

void BatteryMonitorSensors::SetPassengerVoltageOffset(float offset) {
    passenger_voltage_offset_ = offset;
    latest_readings_.passenger_voltage_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetPassengerVoltageSlope(float slope) {
    passenger_voltage_slope_ = slope;
    latest_readings_.passenger_voltage_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetPassengerVoltageOffset() const {
    return passenger_voltage_offset_;
}

float BatteryMonitorSensors::GetPassengerVoltageSlope() const {
    return passenger_voltage_slope_;
}

void BatteryMonitorSensors::SetDriverCurrentOffset(float offset) {
    driver_current_offset_ = offset;
    latest_readings_.driver_current_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetDriverCurrentSlope(float slope) {
    driver_current_slope_ = slope;
    latest_readings_.driver_current_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetDriverCurrentOffset() const {
    return driver_current_offset_;
}

float BatteryMonitorSensors::GetDriverCurrentSlope() const {
    return driver_current_slope_;
}

void BatteryMonitorSensors::SetDriverVoltageOffset(float offset) {
    driver_voltage_offset_ = offset;
    latest_readings_.driver_voltage_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetDriverVoltageSlope(float slope) {
    driver_voltage_slope_ = slope;
    latest_readings_.driver_voltage_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetDriverVoltageOffset() const {
    return driver_voltage_offset_;
}

float BatteryMonitorSensors::GetDriverVoltageSlope() const {
    return driver_voltage_slope_;
}

void BatteryMonitorSensors::SetAlternatorCurrentOffset(float offset) {
    alternator_current_offset_ = offset;
    latest_readings_.alternator_current_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetAlternatorCurrentSlope(float slope) {
    alternator_current_slope_ = slope;
    latest_readings_.alternator_current_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetAlternatorCurrentOffset() const {
    return alternator_current_offset_;
}

float BatteryMonitorSensors::GetAlternatorCurrentSlope() const {
    return alternator_current_slope_;
}

void BatteryMonitorSensors::SetAlternatorVoltageOffset(float offset) {
    alternator_voltage_offset_ = offset;
    latest_readings_.alternator_voltage_offset = offset;
    SaveCalibration();
}

void BatteryMonitorSensors::SetAlternatorVoltageSlope(float slope) {
    alternator_voltage_slope_ = slope;
    latest_readings_.alternator_voltage_slope = slope;
    SaveCalibration();
}

float BatteryMonitorSensors::GetAlternatorVoltageOffset() const {
    return alternator_voltage_offset_;
}

float BatteryMonitorSensors::GetAlternatorVoltageSlope() const {
    return alternator_voltage_slope_;
}

BatteryReadings BatteryMonitorSensors::Sample() {
    const int64_t now_us = esp_timer_get_time();
    const float delta_seconds = last_sample_time_us_ == 0 ? 0.0f : static_cast<float>(now_us - last_sample_time_us_) / 1000000.0f;
    last_sample_time_us_ = now_us;

    latest_readings_.alternator_current_offset = alternator_current_offset_;
    latest_readings_.alternator_current_slope = alternator_current_slope_;
    latest_readings_.alternator_voltage_offset = alternator_voltage_offset_;
    latest_readings_.alternator_voltage_slope = alternator_voltage_slope_;
    latest_readings_.alternator_voltage = alternator_voltage.Read() * alternator_voltage_slope_ - alternator_voltage_offset_;
    latest_readings_.alternator_current = alternator_current.Read() * alternator_current_slope_ - alternator_current_offset_;
    if (latest_readings_.alternator_current > latest_readings_.alternator_current_max) {
        latest_readings_.alternator_current_max = latest_readings_.alternator_current;
    }
    if (latest_readings_.alternator_voltage > latest_readings_.alternator_voltage_max) {
        latest_readings_.alternator_voltage_max = latest_readings_.alternator_voltage;
    }
    if (latest_readings_.alternator_current >= kMinimumTrackedCurrentAmps && delta_seconds > 0.0f) {
        latest_readings_.alternator_energy_joules += latest_readings_.alternator_voltage * latest_readings_.alternator_current * delta_seconds;
    }

    latest_readings_.passenger_voltage_offset = passenger_voltage_offset_;
    latest_readings_.passenger_voltage_slope = passenger_voltage_slope_;
    latest_readings_.passenger_current_offset = passenger_current_offset_;
    latest_readings_.passenger_current_slope = passenger_current_slope_;
    latest_readings_.passenger_voltage = passenger_voltage.Read() * passenger_voltage_slope_ - passenger_voltage_offset_;
    latest_readings_.passenger_current = passenger_current.Read() * passenger_current_slope_ - passenger_current_offset_;

    latest_readings_.driver_voltage_offset = driver_voltage_offset_;
    latest_readings_.driver_voltage_slope = driver_voltage_slope_;
    latest_readings_.driver_current_offset = driver_current_offset_;
    latest_readings_.driver_current_slope = driver_current_slope_;
    latest_readings_.driver_voltage = driver_voltage.Read() * driver_voltage_slope_ - driver_voltage_offset_;

    driver_current_samples_[0] = driver_current.Read() * driver_current_slope_ - driver_current_offset_;
    float driver_current_average = 0.0f;
    for (int i = kCurrentAverageWindow - 1; i > 0; --i) {
        driver_current_average += driver_current_samples_[i];
        driver_current_samples_[i] = driver_current_samples_[i - 1];
    }
    driver_current_average += driver_current_samples_[0];
    latest_readings_.driver_current = driver_current_average / kCurrentAverageWindow;
    return latest_readings_;
}

const BatteryReadings& BatteryMonitorSensors::LatestReadings() const {
    return latest_readings_;
}

void BatteryMonitorSensors::ResetAlternatorPeaks() {
    latest_readings_.alternator_current_max = 0.0f;
    latest_readings_.alternator_voltage_max = 0.0f;
    latest_readings_.alternator_energy_joules = 0.0f;
}

esp_err_t BatteryMonitorSensors::LoadCalibration() {
    nvs_handle_t handle;
    esp_err_t result = nvs_open(kCalibrationNamespace, NVS_READONLY, &handle);
    if (result == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;
    }
    if (result != ESP_OK) {
        return result;
    }

    LoadFloatIfPresent(handle, kPassengerCurrentOffsetKey, passenger_current_offset_);
    LoadFloatIfPresent(handle, kPassengerCurrentSlopeKey, passenger_current_slope_);
    LoadFloatIfPresent(handle, kPassengerVoltageOffsetKey, passenger_voltage_offset_);
    LoadFloatIfPresent(handle, kPassengerVoltageSlopeKey, passenger_voltage_slope_);
    LoadFloatIfPresent(handle, kDriverCurrentOffsetKey, driver_current_offset_);
    LoadFloatIfPresent(handle, kDriverCurrentSlopeKey, driver_current_slope_);
    LoadFloatIfPresent(handle, kDriverVoltageOffsetKey, driver_voltage_offset_);
    LoadFloatIfPresent(handle, kDriverVoltageSlopeKey, driver_voltage_slope_);
    LoadFloatIfPresent(handle, kAlternatorCurrentOffsetKey, alternator_current_offset_);
    LoadFloatIfPresent(handle, kAlternatorCurrentSlopeKey, alternator_current_slope_);
    LoadFloatIfPresent(handle, kAlternatorVoltageOffsetKey, alternator_voltage_offset_);
    LoadFloatIfPresent(handle, kAlternatorVoltageSlopeKey, alternator_voltage_slope_);

    latest_readings_.passenger_current_offset = passenger_current_offset_;
    latest_readings_.passenger_current_slope = passenger_current_slope_;
    latest_readings_.passenger_voltage_offset = passenger_voltage_offset_;
    latest_readings_.passenger_voltage_slope = passenger_voltage_slope_;
    latest_readings_.driver_current_offset = driver_current_offset_;
    latest_readings_.driver_current_slope = driver_current_slope_;
    latest_readings_.driver_voltage_offset = driver_voltage_offset_;
    latest_readings_.driver_voltage_slope = driver_voltage_slope_;
    latest_readings_.alternator_current_offset = alternator_current_offset_;
    latest_readings_.alternator_current_slope = alternator_current_slope_;
    latest_readings_.alternator_voltage_offset = alternator_voltage_offset_;
    latest_readings_.alternator_voltage_slope = alternator_voltage_slope_;

    nvs_close(handle);
    return ESP_OK;
}

void BatteryMonitorSensors::SaveCalibration() {
    if (!storage_ready_) {
        return;
    }

    nvs_handle_t handle;
    esp_err_t result = nvs_open(kCalibrationNamespace, NVS_READWRITE, &handle);
    if (result != ESP_OK) {
        ESP_LOGE(kLogTag, "Failed to open calibration namespace: %s", esp_err_to_name(result));
        return;
    }

    SaveFloat(handle, kPassengerCurrentOffsetKey, passenger_current_offset_);
    SaveFloat(handle, kPassengerCurrentSlopeKey, passenger_current_slope_);
    SaveFloat(handle, kPassengerVoltageOffsetKey, passenger_voltage_offset_);
    SaveFloat(handle, kPassengerVoltageSlopeKey, passenger_voltage_slope_);
    SaveFloat(handle, kDriverCurrentOffsetKey, driver_current_offset_);
    SaveFloat(handle, kDriverCurrentSlopeKey, driver_current_slope_);
    SaveFloat(handle, kDriverVoltageOffsetKey, driver_voltage_offset_);
    SaveFloat(handle, kDriverVoltageSlopeKey, driver_voltage_slope_);
    SaveFloat(handle, kAlternatorCurrentOffsetKey, alternator_current_offset_);
    SaveFloat(handle, kAlternatorCurrentSlopeKey, alternator_current_slope_);
    SaveFloat(handle, kAlternatorVoltageOffsetKey, alternator_voltage_offset_);
    SaveFloat(handle, kAlternatorVoltageSlopeKey, alternator_voltage_slope_);

    result = nvs_commit(handle);
    if (result != ESP_OK) {
        ESP_LOGE(kLogTag, "Failed to commit calibration: %s", esp_err_to_name(result));
    }
    nvs_close(handle);
}
