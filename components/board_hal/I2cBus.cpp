#include "I2cBus.hpp"

#include "BoardConfig.hpp"

I2cBus::I2cBus(gpio_num_t sda_pin, gpio_num_t scl_pin, i2c_port_num_t port)
    : sda_pin_(sda_pin), scl_pin_(scl_pin), port_(port) {}

esp_err_t I2cBus::Init() {
    if (handle_ != nullptr) {
        return ESP_ERR_INVALID_STATE;
    }

    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = port_;
    bus_config.sda_io_num = sda_pin_;
    bus_config.scl_io_num = scl_pin_;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = board::kI2cIgnoreCount;
    bus_config.flags.enable_internal_pullup = board::kI2cPullupsEnabled;
    return i2c_new_master_bus(&bus_config, &handle_);
}

i2c_master_bus_handle_t I2cBus::Handle() const {
    return handle_;
}
