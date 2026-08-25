#pragma once

#include "PinController.hpp"

extern DigitalInput left_button;
extern DigitalInput right_button;
extern DigitalInput diesel_heater;
extern DigitalInput electric_heater;
extern DigitalInput engine_heater;
extern DigitalInput water_pump;
extern DigitalInput cabin_pump;
extern DigitalInput engine_pump;

extern AnalogInput alternator_current;
extern AnalogInput alternator_voltage;
extern AnalogInput lion_1_voltage;
extern AnalogInput lion_2_voltage;
extern AnalogInput lion_2_current;
extern AnalogInput lion_1_current;

void InitInputs();
