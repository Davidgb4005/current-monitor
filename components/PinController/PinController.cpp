#include "PinController.hpp"

bool PinController::isr_init = false;

PinController::PinController()
{

    if (isr_init == false)
    {
        gpio_install_isr_service(0);
    }
}

int8_t PinController::SetPinConfig(uint8_t pin, PIN_TYPE pin_type)
{
    if (this->pin_array[pin].pin_enabled == true)
    {
        ESP_LOGI("PinController", "Pin Already Used: %d", pin);
        return -1;
    }
    else
    {
        this->pin_array[pin].pin_type = pin_type;
        this->pin_array[pin].pin_enabled = true;
        return 0;
    }
}

int8_t PinController::ResetPinConfig(uint8_t pin)
{
    if (this->pin_array[pin].pin_enabled == false)
    {
        ESP_LOGI("PinController", "Pin Already Unused: %d", pin);
        return -1;
    }
    else
    {
        this->pin_array[pin].pin_type = UNUSED;
        this->pin_array[pin].pin_enabled = false;
        this->pin_array[pin].pwm_channel = 0;
        return 0;
    }
}

int8_t PinController::SetPwmPin(uint8_t pin, PIN_TYPE type, ledc_timer_bit_t pwm_res, uint16_t pwm_freq, uint8_t channel)
{
    uint64_t bit_mask = 0;

    switch ((int)pin)
    {
    case 0:
        bit_mask = (1ULL << GPIO_NUM_0);
        break;
    case 1:
        bit_mask = (1ULL << GPIO_NUM_1);
        break;
    case 2:
        bit_mask = (1ULL << GPIO_NUM_2);
        break;
    case 3:
        bit_mask = (1ULL << GPIO_NUM_3);
        break;
    case 4:
        bit_mask = (1ULL << GPIO_NUM_4);
        break;
    case 5:
        bit_mask = (1ULL << GPIO_NUM_5);
        break;
    case 6:
        bit_mask = (1ULL << GPIO_NUM_6);
        break;
    case 7:
        bit_mask = (1ULL << GPIO_NUM_7);
        break;
    case 8:
        bit_mask = (1ULL << GPIO_NUM_8);
        break;
    case 9:
        bit_mask = (1ULL << GPIO_NUM_9);
        break;
    case 10:
        bit_mask = (1ULL << GPIO_NUM_10);
        break;
    case 11:
        bit_mask = (1ULL << GPIO_NUM_11);
        break;
    case 12:
        bit_mask = (1ULL << GPIO_NUM_12);
        break;
    case 13:
        bit_mask = (1ULL << GPIO_NUM_13);
        break;
    case 14:
        bit_mask = (1ULL << GPIO_NUM_14);
        break;
    case 15:
        bit_mask = (1ULL << GPIO_NUM_15);
        break;
    case 16:
        bit_mask = (1ULL << GPIO_NUM_16);
        break;
    case 17:
        bit_mask = (1ULL << GPIO_NUM_17);
        break;
    case 18:
        bit_mask = (1ULL << GPIO_NUM_18);
        break;
    case 19:
        bit_mask = (1ULL << GPIO_NUM_19);
        break;
    case 20:
        bit_mask = (1ULL << GPIO_NUM_20);
        break;
    case 21:
        bit_mask = (1ULL << GPIO_NUM_21);
        break;
    case 22:
        bit_mask = (1ULL << GPIO_NUM_22);
        break;
    case 23:
        bit_mask = (1ULL << GPIO_NUM_23);
        break;
    case 24:
        bit_mask = (1ULL << GPIO_NUM_24);
        break;
    case 25:
        bit_mask = (1ULL << GPIO_NUM_25);
        break;
    case 26:
        bit_mask = (1ULL << GPIO_NUM_26);
        break;
    case 27:
        bit_mask = (1ULL << GPIO_NUM_27);
        break;
    case 28:
        bit_mask = (1ULL << GPIO_NUM_28);
        break;
    case 29:
        bit_mask = (1ULL << GPIO_NUM_29);
        break;
    case 30:
        bit_mask = (1ULL << GPIO_NUM_30);
        break;
    default:
        ESP_LOGI("PwmController", "Invalid Pin: %d", pin);
        return -1;
    }

    if (channel > 7)
    {
        ESP_LOGI("PinController", "Invalid Channel: %d", channel);
        return -1;
    }

    if (SetPinConfig(pin, type) < 0)
    {
        return -1;
    }
    else
    {
        ledc_timer_config_t ledc_timer = {};
        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num = LEDC_TIMER_0;
        ledc_timer.duty_resolution = pwm_res;
        ledc_timer.freq_hz = pwm_freq;
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;

        ledc_timer_config(&ledc_timer);

        pin_array[pin].pwm_channel = channel;
        ledc_channel_config_t ledc_channel = {};
        ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel.channel = (ledc_channel_t)channel;
        ledc_channel.timer_sel = LEDC_TIMER_0;
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.gpio_num = pin;
        ledc_channel.duty = 0;
        ledc_channel.hpoint = 0;

        ledc_channel_config(&ledc_channel);
        return 0;
    }
}

int8_t PinController::WritePwm(uint8_t pin, uint16_t duty)
{
    ledc_channel_t channel = (ledc_channel_t)pin_array[pin].pwm_channel;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
    return 0;
}

int8_t PinController::SetUartPin(uint8_t rx_pin, uint8_t tx_pin, uint16_t baud,
                                 uint16_t rx_buf_size, uint16_t tx_buf_size)
{

    if (SetPinConfig(rx_pin, UART_RX) < 0 && SetPinConfig(tx_pin, UART_TX) < 0)
    {
        return -1;
    }
    else
    {
        uart_config_t uart_config = {};
        uart_config.baud_rate = baud;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.source_clk = UART_SCLK_DEFAULT;

        uart_driver_install(UART_NUM_1, 512, 512, 0, NULL, 0);
        uart_param_config(UART_NUM_1, &uart_config);
        uart_set_pin(UART_NUM_1, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        return 0;
    }
}

uint16_t PinController::ReadUart(uint8_t *rx_buffer, uint16_t len, uint16_t time_out)
{
    return uart_read_bytes(UART_NUM_1, rx_buffer, len, time_out / portTICK_PERIOD_MS);
}
uint16_t PinController::WriteUart(uint8_t *tx_buffer, uint16_t len)
{
    return uart_write_bytes(UART_NUM_1, (const char *)tx_buffer, len);
}

#define DEBOUNCE 20000
void EncoderIncrement(void *arg)
{
    Encoder *enc = (Encoder *)arg;
    uint8_t level_a = gpio_get_level((gpio_num_t)18);
    uint8_t level_b = gpio_get_level((gpio_num_t)19) << 1;
    static uint8_t enc_state = level_a | level_b;
    switch (enc_state)
    {
    case 0:
        if ((level_a | level_b) == 1)
        {
            enc->counter++;
            enc_state = 1;
        }
        else if ((level_a | level_b) == 2)
        {
            enc->counter--;
            enc_state = 2;
        }
        break;
    case 1:
        if ((level_a | level_b) == 0)
        {
            enc->counter--;
            enc_state = 0;
        }
        else if ((level_a | level_b) == 3)
        {
            enc->counter++;
            enc_state = 3;
        }
        break;
    case 2:
        if ((level_a | level_b) == 0)
        {
            enc->counter++;
            enc_state = 0;
        }
        else if ((level_a | level_b) == 3)
        {
            enc->counter--;
            enc_state = 3;
        }
        break;
    case 3:
        if ((level_a | level_b) == 2)
        {
            enc->counter++;
            enc_state = 2;
        }
        else if ((level_a | level_b) == 1)
        {
            enc->counter--;
            enc_state = 1;
        }
        break;
    default:
        break;
    }
}

DigitalInput::DigitalInput(uint8_t pin,
                           DIGITAL_IN_PIN_TYPE type,
                           DEBOUNCE_TYPE debounce_type,
                           uint16_t debounce_time,
                           INTERRUPT_TYPE interrupt_type,
                           void (*callback)(void *),
                           void *arg)
{
    this->pin = static_cast<gpio_num_t>(pin);
    this->pin_type = type;
    this->debounce_type = debounce_type;
    this->debounce_time = debounce_time * 1000;
    this->interrupt_type = interrupt_type;
    this->callback = callback;
    this->arg = arg;
}
DigitalInput::~DigitalInput(){}
void DigitalInput::Init()
{
    this->last = esp_timer_get_time();

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << this->pin);
    io_conf.mode = GPIO_MODE_INPUT;

    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    switch (pin_type)
    {
    case DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLUP:
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        break;
    case DIGITAL_IN_PIN_TYPE::DIGITAL_IN_PULLDOWN:
        io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
        break;
    default:
        break;
    }

    switch (this->interrupt_type)
    {
    case INTERRUPT_TYPE::DISABLE:
        io_conf.intr_type = GPIO_INTR_DISABLE;
        break;
    case INTERRUPT_TYPE::RISING_EDGE:
        io_conf.intr_type = GPIO_INTR_POSEDGE;
        break;
    case INTERRUPT_TYPE::FALLING_EDGE:
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
        break;
    case INTERRUPT_TYPE::EDGE:
        io_conf.intr_type = GPIO_INTR_ANYEDGE;
        break;
    case INTERRUPT_TYPE::LOW:
        io_conf.intr_type = GPIO_INTR_LOW_LEVEL;
        break;
    case INTERRUPT_TYPE::HIGH:
        io_conf.intr_type = GPIO_INTR_HIGH_LEVEL;
        break;
    default:
        io_conf.intr_type = GPIO_INTR_DISABLE;
        break;
    }
    gpio_config(&io_conf);
    if (this->interrupt_type != INTERRUPT_TYPE::DISABLE && callback != nullptr)
    {
        gpio_isr_handler_add(this->pin, callback, arg);
    }
}
bool DigitalInput::Read()
{
    uint64_t now = esp_timer_get_time();

    switch (debounce_type)
    {
    case DEBOUNCE_TYPE::DISABLE:
        gpio_level = gpio_get_level(pin);
        break;
    case DEBOUNCE_TYPE::RISE_DEBOUNCE:
        if (gpio_level == false && gpio_get_level(pin))
        {
            if ((now - last) >= debounce_time)
            {
                gpio_level = true;
            }
        }
        else if (!gpio_get_level(pin))
        {
            last = now;
            gpio_level = false;
        }
        break;
    case DEBOUNCE_TYPE::FALL_DEBOUNCE:
        if (gpio_level == true && !gpio_get_level(pin))
        {
            if ((now - last) >= debounce_time)
            {
                gpio_level = false;
            }
        }
        else if (gpio_get_level(pin))
        {
            last = now;
            gpio_level = true;
        }
        break;
    case DEBOUNCE_TYPE::RISE_FALL_DEBOUNCE:
        if (gpio_level == true && !gpio_get_level(pin))
        {
            if ((now - last) >= debounce_time)
            {
                gpio_level = false;
            }
        }
        else if (gpio_level == false && gpio_get_level(pin))
        {
            if ((now - last) >= debounce_time)
            {
                gpio_level = true;
            }
        }
        else
        {
            last = now;
        }
        break;
    }
    return gpio_level;
}
void DigitalInput::SetEdge()
{
    Read();
    if (prev_gpio_level == true && gpio_level == false)
    {
        falling_edge = true;
        ESP_LOGI("I2C", "Falling Edge: %d", true);
    }
    else
    {
        falling_edge = false;
    }
    if (prev_gpio_level == false && gpio_level == true)
    {
        rising_edge = true;
        ESP_LOGI("I2C", "Rising Edge: %d", true);
    }
    else
    {
        rising_edge = false;
        falling_edge = false;
    }
    prev_gpio_level = gpio_level;
}
bool DigitalInput::ReadRisingEdge()
{
    return rising_edge;
}
bool DigitalInput::ReadFallingEdge()
{
    return falling_edge;
}

DigitalOutput::DigitalOutput(uint8_t pin)

{
    this->pin = static_cast<gpio_num_t>(pin);
}
DigitalOutput::~DigitalOutput(){}
void DigitalOutput::Init()
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << this->pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}
bool DigitalOutput::Read()
{
    return state;
}
void DigitalOutput::Write(bool val)

{
    state = val;
    gpio_set_level(pin, state);
}

AnalogInput::AnalogInput(uint8_t pin,
                         adc_oneshot_unit_handle_t *adc_handle,
                         adc_atten_t attenuation,
                         adc_bitwidth_t bitwidth)
{

    this->pin = static_cast<adc_channel_t>(pin);
    this->attenuation = attenuation;
    this->bitwidth = bitwidth;
    this->adc_handle = adc_handle;
}
AnalogInput::~AnalogInput(){}
void AnalogInput::Init()
{
    adc_oneshot_chan_cfg_t config = {};
    config.atten = this->attenuation;
    config.bitwidth = this->bitwidth;
    adc_oneshot_config_channel(*adc_handle, this->pin, &config);
}
int16_t AnalogInput::Read()
{

    uint16_t value = 0;
    adc_oneshot_read(*adc_handle, pin, (int *)&value);
    return value * 1.265;
}
