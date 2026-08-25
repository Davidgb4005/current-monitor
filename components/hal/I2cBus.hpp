#pragma once

#include "driver/i2c_master.h"

class I2cBus {
public:
    I2cBus(gpio_num_t sda_pin, gpio_num_t scl_pin, i2c_port_num_t port);
    ~I2cBus() = default;

    esp_err_t Init();
    i2c_master_bus_handle_t Handle() const;

private:
    gpio_num_t sda_pin_;
    gpio_num_t scl_pin_;
    i2c_port_num_t port_;
    i2c_master_bus_handle_t handle_ = nullptr;
};
