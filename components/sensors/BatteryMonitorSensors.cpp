#include "BatteryMonitorSensors.hpp"

#include "Inputs.hpp"

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

BatteryReadings BatteryMonitorSensors::Sample() {
    latest_readings_.alternator_current_raw = alternator_current.Read();
    latest_readings_.alternator_voltage_raw = alternator_voltage.Read();
    if (latest_readings_.alternator_current_raw > latest_readings_.alternator_current_max) {
        latest_readings_.alternator_current_max = latest_readings_.alternator_current_raw;
    }
    if (latest_readings_.alternator_voltage_raw > latest_readings_.alternator_voltage_max) {
        latest_readings_.alternator_voltage_max = latest_readings_.alternator_voltage_raw;
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
