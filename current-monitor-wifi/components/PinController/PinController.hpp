#pragma once
#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h" // Main LEDC API
#include "driver/uart.h"
#include "esp_timer.h"
#ifdef CONFIG_IDF_TARGET_ESP32C6 // ESP32-C6
#define PIN_AMT 21
#else
#define PIN_AMT 21
#endif

enum class INTERRUPT_TYPE
{
    DISABLE,
    RISING_EDGE,
    FALLING_EDGE,
    EDGE,
    LOW,
    HIGH
};

enum class DIGITAL_IN_PIN_TYPE
{
    NONE,                // done
    DIGITAL_IN_PULLUP,   // done
    DIGITAL_IN_PULLDOWN, // done
};

enum class DEBOUNCE_TYPE
{
    DISABLE,
    RISE_DEBOUNCE,
    FALL_DEBOUNCE,
    RISE_FALL_DEBOUNCE
};

enum PIN_TYPE
{
    UNUSED,               // done
    DIGITAL_OUT,          // done
    DIGITAL_OUT_PULLDOWN, // done
    DIGITAL_OUT_PULLUP,   // done
    PWM,                  // done
    ANALOG_IN,            // done
    UART_TX,
    UART_RX,
    ENCODER,
    I2C_SDA, // done
    I2C_SCL, // done
    SPI_MOSI,
    SPI_MISO,
    SPI_CLK,
    SPI_CS

};

struct PinConfig
{
    PIN_TYPE pin_type;
    bool pin_enabled = false;
    uint8_t pwm_channel = 0;
};

struct Encoder
{
    int32_t counter = 0;
};

class PinController
{
private:
    PinConfig pin_array[PIN_AMT];

public:
    static bool isr_init;

    PinController();
    ~PinController();

    int8_t SetPinConfig(uint8_t pin, PIN_TYPE pin_type);
    int8_t ResetPinConfig(uint8_t pin);

    int8_t SetPwmPin(uint8_t pin, PIN_TYPE type, ledc_timer_bit_t pwm_res, uint16_t pwm_freq, uint8_t channel);
    int8_t WritePwm(uint8_t pin, uint16_t duty);

    int8_t SetUartPin(uint8_t rx_pin, uint8_t tx_pin, uint16_t baud = 9600,
                      uint16_t rx_buf_size = 0, uint16_t tx_buf_size = 0);
    uint16_t ReadUart(uint8_t *rx_buffer, uint16_t len, uint16_t time_out = 10);
    uint16_t WriteUart(uint8_t *tx_buffer, uint16_t len);
};

void EncoderIncrement(void *arg);
void EncoderIncrementB(void *arg);

class DigitalInput
{

private:
    uint32_t debounce_time;
    DEBOUNCE_TYPE debounce_type;
    gpio_num_t pin;
    DIGITAL_IN_PIN_TYPE pin_type;
    INTERRUPT_TYPE interrupt_type;
    uint64_t last;
    void (*callback)(void *);
    void *arg;
    bool gpio_level = false;
    bool prev_gpio_level = false;
    bool rising_edge = false;
    bool falling_edge = false;

public:
    DigitalInput(uint8_t pin,
                 DIGITAL_IN_PIN_TYPE type = DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN,
                 DEBOUNCE_TYPE debounce_type = DEBOUNCE_TYPE::DISABLE,
                 uint16_t debounce_time = 0,
                 INTERRUPT_TYPE interrupt_type = INTERRUPT_TYPE::DISABLE,
                 void (*callback)(void *) = nullptr,
                 void *arg = nullptr);

    ~DigitalInput();
    void Init();
    bool Read();
    void SetEdge();
    bool ReadRisingEdge();
    bool ReadFallingEdge();
};

class DigitalOutput
{

private:
    gpio_num_t pin;
    bool state;

public:
    DigitalOutput(uint8_t pin);
    ~DigitalOutput();
    void Init();
    void Write(bool val);
    bool Read();
};

class AnalogInput
{
private:
    adc_channel_t pin;
    adc_atten_t attenuation;
    adc_bitwidth_t bitwidth;
    adc_oneshot_unit_handle_t *adc_handle = nullptr;

public:
    AnalogInput(uint8_t pin,
                adc_oneshot_unit_handle_t *adc_handle,
                adc_atten_t attenuation = ADC_ATTEN_DB_12,
                adc_bitwidth_t bitwidth = ADC_BITWIDTH_12);
    ~AnalogInput();
    void Init();
    int16_t Read();
};
