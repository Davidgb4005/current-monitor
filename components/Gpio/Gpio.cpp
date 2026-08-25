#include "Gpio.hpp"
// DIGITAL INPUT SETUP
DigitalInput left_button(21, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput right_button(22, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput diesel_heater(9, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput electric_heater(18, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput engine_heater(19, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput water_pump(13, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput cabin_pump(12, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
// DIGITAL OUTPUT SETUP
// DigitalOutput pin_1(10);

// Analog Input Setup
static adc_oneshot_unit_handle_t adc_handle = nullptr;
AnalogInput alternator_current(1, &adc_handle);
AnalogInput alternator_voltage(0, &adc_handle);
AnalogInput lion_1_voltage(4, &adc_handle);
AnalogInput lion_2_voltage(5, &adc_handle);
AnalogInput lion_2_current(2, &adc_handle);
AnalogInput lion_1_current(3, &adc_handle);

// Setup Functions
int8_t Analog_Input_Begin()
{
    if (adc_handle == nullptr)
    {
        adc_oneshot_unit_init_cfg_t init_cfg = {};
        init_cfg.unit_id = ADC_UNIT_1;
        adc_oneshot_new_unit(&init_cfg, &adc_handle);
        return 0;
    }
    return -1;
}
void InitGpio()
{
    Analog_Input_Begin();
    left_button.Init();
    right_button.Init();
    diesel_heater.Init();
    electric_heater.Init();
    engine_heater.Init();
    water_pump.Init();
    cabin_pump.Init();
    alternator_current.Init();
    alternator_voltage.Init();
    lion_1_voltage.Init();
    lion_2_voltage.Init();
    lion_2_current.Init();
    lion_1_current.Init();
}
