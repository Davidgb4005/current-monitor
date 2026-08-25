#include "BatteryMonitorSensors.hpp"

#include "Inputs.hpp"
#include "esp_timer.h"

void BatteryMonitorSensors::SetLion1Offset(int offset) {
    lion_1_offset_ = offset;
}

void BatteryMonitorSensors::SetLion2Offset(int offset) {
    lion_2_offset_ = offset;
}

int BatteryMonitorSensors::GetLion1Offset() const {
    return lion_1_offset_;
}

int BatteryMonitorSensors::GetLion2Offset() const {
    return lion_2_offset_;
}

void BatteryMonitorSensors::SetAlternatorCurrentOffset(float offset) {
    alternator_current_offset_ = offset;
    latest_readings_.alternator_current_offset = offset;
}

void BatteryMonitorSensors::SetAlternatorCurrentSlope(float slope) {
    alternator_current_slope_ = slope;
    latest_readings_.alternator_current_slope = slope;
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
}

void BatteryMonitorSensors::SetAlternatorVoltageSlope(float slope) {
    alternator_voltage_slope_ = slope;
    latest_readings_.alternator_voltage_slope = slope;
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
    latest_readings_.lion_1_voltage = lion_1_voltage.Read() * kVoltageScale;
    latest_readings_.lion_2_voltage = lion_2_voltage.Read() * kVoltageScale;
    latest_readings_.lion_1_current = lion_1_current.Read() * kCurrentScale - lion_1_offset_;

    lion_2_current_samples_[0] = lion_2_current.Read() * kCurrentScale - lion_2_offset_;
    float lion_2_average = 0.0f;
    for (int i = kCurrentAverageWindow - 1; i > 0; --i) {
        lion_2_average += lion_2_current_samples_[i];
        lion_2_current_samples_[i] = lion_2_current_samples_[i - 1];
    }
    lion_2_average += lion_2_current_samples_[0];
    latest_readings_.lion_2_current = lion_2_average / kCurrentAverageWindow;
    return latest_readings_;
}

const BatteryReadings& BatteryMonitorSensors::LatestReadings() const {
    return latest_readings_;
}

void BatteryMonitorSensors::ResetAlternatorPeaks() {
    latest_readings_.alternator_current_max = 0.0f;
    latest_readings_.alternator_voltage_max = 0.0f;
}
