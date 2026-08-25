#pragma once

#include <cstdint>

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
    float driver_voltage = 0.0f;
    float passenger_current = 0.0f;
    float driver_current = 0.0f;
};

class BatteryMonitorSensors {
public:
    void SetPassengerCurrentOffset(int offset);
    void SetDriverCurrentOffset(int offset);
    int GetPassengerCurrentOffset() const;
    int GetDriverCurrentOffset() const;

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
    static constexpr float kVoltageScale = 15.0f / 4096.0f;
    static constexpr float kCurrentScale = 0.173f;
    static constexpr float kMinimumTrackedCurrentAmps = 5.0f;
    static constexpr int kCurrentAverageWindow = 20;

    float driver_current_samples_[kCurrentAverageWindow] = {};
    int passenger_current_offset_ = 369;
    int driver_current_offset_ = 369;
    float alternator_current_offset_ = 0.0f;
    float alternator_current_slope_ = kCurrentScale;
    float alternator_voltage_offset_ = 0.0f;
    float alternator_voltage_slope_ = kVoltageScale;
    int64_t last_sample_time_us_ = 0;
    BatteryReadings latest_readings_ = {};
};
