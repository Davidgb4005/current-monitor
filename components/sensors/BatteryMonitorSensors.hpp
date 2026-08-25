#pragma once

#include <cstdint>

struct BatteryReadings {
    float alternator_current_raw = 0.0f;
    float alternator_voltage_raw = 0.0f;
    float lion_1_voltage = 0.0f;
    float lion_2_voltage = 0.0f;
    float lion_1_current = 0.0f;
    float lion_2_current = 0.0f;
};

class BatteryMonitorSensors {
public:
    void SetLion1Offset(int offset);
    void SetLion2Offset(int offset);
    int GetLion1Offset() const;
    int GetLion2Offset() const;

    BatteryReadings Sample();

private:
    static constexpr float kVoltageScale = 15.0f / 4096.0f;
    static constexpr float kCurrentScale = 0.173f;
    static constexpr int kCurrentAverageWindow = 20;

    float lion_2_current_samples_[kCurrentAverageWindow] = {};
    int lion_1_offset_ = 369;
    int lion_2_offset_ = 369;
};
