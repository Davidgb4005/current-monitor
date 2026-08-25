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

constexpr uint8_t kLeftButtonPin = 20;
constexpr uint8_t kRightButtonPin = 21;
constexpr uint8_t kDieselHeaterPin = 9;
constexpr uint8_t kElectricHeaterPin = 18;
constexpr uint8_t kEngineHeaterPin = 19;
constexpr uint8_t kWaterPumpPin = 13;
constexpr uint8_t kCabinPumpPin = 12;

constexpr uint8_t kAlternatorCurrentChannel = 2;
constexpr uint8_t kAlternatorVoltageChannel = 0;
constexpr uint8_t kPassengerVoltageChannel = 4;
constexpr uint8_t kDriverVoltageChannel = 5;
constexpr uint8_t kDriverCurrentChannel = 3;
constexpr uint8_t kPassengerCurrentChannel = 1;

}  // namespace board
