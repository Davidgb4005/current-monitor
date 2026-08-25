#include "BatteryMonitorSensors.hpp"

#include "Inputs.hpp"
#include "esp_timer.h"

void BatteryMonitorSensors::SetPassengerCurrentOffset(int offset) {
    passenger_current_offset_ = offset;
}

void BatteryMonitorSensors::SetDriverCurrentOffset(int offset) {
    driver_current_offset_ = offset;
}

int BatteryMonitorSensors::GetPassengerCurrentOffset() const {
    return passenger_current_offset_;
}

int BatteryMonitorSensors::GetDriverCurrentOffset() const {
    return driver_current_offset_;
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
    latest_readings_.passenger_voltage = passenger_voltage.Read() * kVoltageScale;
    latest_readings_.driver_voltage = driver_voltage.Read() * kVoltageScale;
    latest_readings_.passenger_current = passenger_current.Read() * kCurrentScale - passenger_current_offset_;

    driver_current_samples_[0] = driver_current.Read() * kCurrentScale - driver_current_offset_;
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
