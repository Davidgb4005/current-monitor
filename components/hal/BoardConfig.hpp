#pragma once

#include <cstdint>

#include "driver/i2c_master.h"

namespace board {

constexpr uint8_t kI2cSdaPin = 15;
constexpr uint8_t kI2cSclPin = 23;
constexpr i2c_port_num_t kI2cPort = I2C_NUM_0;
constexpr i2c_addr_bit_len_t kLcdAddressLength = I2C_ADDR_BIT_LEN_7;
constexpr uint8_t kLcdAddress = 0x27;
constexpr uint32_t kI2cClockHz = 20000;
constexpr uint32_t kI2cIgnoreCount = 10;
constexpr bool kI2cPullupsEnabled = true;

}  // namespace board
