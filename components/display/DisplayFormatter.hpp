#pragma once

#include <cstdint>

namespace display {

int8_t EncodeInt(char* buffer, int32_t value);
int8_t EncodeFloat(char* buffer, float value, int positions);

}  // namespace display
