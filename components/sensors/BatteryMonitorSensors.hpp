#pragma once

#include <cstdint>

#include "esp_err.h"

struct BatteryReadings {
    float alternator_current = 0.0f;
    float alternator_voltage = 0.0f;
    float alternator_current_max = 0.0f;
    float alternator_voltage_max = 0.0f;
    float alternator_current_offset = 0.0f;
    float alternator_current_slope = 0.173f;
    float alternator_voltage_offset = 0.0f;
    float alternator_voltage_slope = 15.0f / 4096.0f;
    float alternator_energy_joules = 0.0f;

    float passenger_voltage = 0.0f;
    float passenger_voltage_offset = 0.0f;
    float passenger_voltage_slope = 15.0f / 4096.0f;
    float passenger_current = 0.0f;
    float passenger_current_offset = 369.0f;
    float passenger_current_slope = 0.173f;

    float driver_voltage = 0.0f;
    float driver_voltage_offset = 0.0f;
    float driver_voltage_slope = 15.0f / 4096.0f;
    float driver_current = 0.0f;
    float driver_current_offset = 369.0f;
    float driver_current_slope = 0.173f;
};

class BatteryMonitorSensors {
public:
    esp_err_t Init();

    void SetPassengerCurrentOffset(float offset);
    void SetPassengerCurrentSlope(float slope);
    float GetPassengerCurrentOffset() const;
    float GetPassengerCurrentSlope() const;

    void SetPassengerVoltageOffset(float offset);
    void SetPassengerVoltageSlope(float slope);
    float GetPassengerVoltageOffset() const;
    float GetPassengerVoltageSlope() const;

    void SetDriverCurrentOffset(float offset);
    void SetDriverCurrentSlope(float slope);
    float GetDriverCurrentOffset() const;
    float GetDriverCurrentSlope() const;

    void SetDriverVoltageOffset(float offset);
    void SetDriverVoltageSlope(float slope);
    float GetDriverVoltageOffset() const;
    float GetDriverVoltageSlope() const;

    void SetAlternatorCurrentOffset(float offset);
    void SetAlternatorCurrentSlope(float slope);
    float GetAlternatorCurrentOffset() const;
    float GetAlternatorCurrentSlope() const;

    void SetAlternatorVoltageOffset(float offset);
    void SetAlternatorVoltageSlope(float slope);
    float GetAlternatorVoltageOffset() const;
    float GetAlternatorVoltageSlope() const;

    BatteryReadings Sample();
    const BatteryReadings& LatestReadings() const;
    void ResetAlternatorPeaks();

private:
    esp_err_t LoadCalibration();
    void SaveCalibration();

    static constexpr float kVoltageScale = 15.0f / 4096.0f;
    static constexpr float kCurrentScale = 0.173f;
    static constexpr float kMinimumTrackedCurrentAmps = 5.0f;
    static constexpr int kCurrentAverageWindow = 20;

    float driver_current_samples_[kCurrentAverageWindow] = {};
    float passenger_current_offset_ = 369.0f;
    float passenger_current_slope_ = kCurrentScale;
    float passenger_voltage_offset_ = 0.0f;
    float passenger_voltage_slope_ = kVoltageScale;
    float driver_current_offset_ = 369.0f;
    float driver_current_slope_ = kCurrentScale;
    float driver_voltage_offset_ = 0.0f;
    float driver_voltage_slope_ = kVoltageScale;
    float alternator_current_offset_ = 369.0f;
    float alternator_current_slope_ = kCurrentScale;
    float alternator_voltage_offset_ = 0.0f;
    float alternator_voltage_slope_ = kVoltageScale;
    bool storage_ready_ = false;
    int64_t last_sample_time_us_ = 0;
    BatteryReadings latest_readings_ = {};
};
