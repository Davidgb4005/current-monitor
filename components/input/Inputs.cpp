#include "Inputs.hpp"

#include "BoardConfig.hpp"

DigitalInput left_button(board::kLeftButtonPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput right_button(board::kRightButtonPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput diesel_heater(board::kDieselHeaterPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput electric_heater(board::kElectricHeaterPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput engine_heater(board::kEngineHeaterPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput water_pump(board::kWaterPumpPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput cabin_pump(board::kCabinPumpPin, DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN, DEBOUNCE_TYPE::RISE_DEBOUNCE, 100);
DigitalInput engine_pump(0, DIGITAL_IN_PIN_TYPE::NONE);

static adc_oneshot_unit_handle_t adc_handle = nullptr;
AnalogInput alternator_current(board::kAlternatorCurrentChannel, &adc_handle);
AnalogInput alternator_voltage(board::kAlternatorVoltageChannel, &adc_handle);
AnalogInput lion_1_voltage(board::kLion1VoltageChannel, &adc_handle);
AnalogInput lion_2_voltage(board::kLion2VoltageChannel, &adc_handle);
AnalogInput lion_2_current(board::kLion2CurrentChannel, &adc_handle);
AnalogInput lion_1_current(board::kLion1CurrentChannel, &adc_handle);

namespace {

int8_t InitAnalogInputs() {
    if (adc_handle == nullptr) {
        adc_oneshot_unit_init_cfg_t init_cfg = {};
        init_cfg.unit_id = ADC_UNIT_1;
        adc_oneshot_new_unit(&init_cfg, &adc_handle);
        return 0;
    }
    return -1;
}

}  // namespace

void InitInputs() {
    InitAnalogInputs();
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
