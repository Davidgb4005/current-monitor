#include "DisplayFormatter.hpp"

#include "esp_log.h"

namespace display {

int8_t EncodeInt(char* buffer, int32_t value) {
    uint8_t i = 0;
    int8_t is_neg = 0;
    if (value < 0) {
        is_neg = 1;
        buffer[0] = '-';
        value = value * -1;
    } else if (value == 0) {
        buffer[0] = '0';
        i++;
        return i;
    }

    while (value > 0) {
        buffer[20 - i] = '0' + (value % 10);
        value = value / 10;
        i++;
        if (i > 20) {
            ESP_LOGI("DisplayFormatter", "Conversion Failure");
            return -1;
        }
    }

    if (i < 20) {
        for (int k = 0; k <= i; k++) {
            buffer[k + is_neg] = buffer[20 - i + 1 + k];
        }
    }
    return i + is_neg;
}

int8_t EncodeFloat(char* buffer, float value, int positions) {
    uint8_t temp_buffer[20] = {0};
    uint8_t i = 0;
    int8_t is_neg = 0;
    if (value < 0) {
        is_neg = 1;
        buffer[0] = '-';
        value = value * -1;
    }

    int32_t exponent = 10;
    for (int k = 0; k < positions - 1; k++) {
        exponent *= 10;
    }
    int32_t remainder = value * exponent;
    int32_t quotient = value;

    for (int k = 0; k < positions; k++) {
        temp_buffer[20 - i] = '0' + (remainder % 10);
        remainder = remainder / 10;
        i++;

        if (i > 20) {
            ESP_LOGI("DisplayFormatter", "Conversion Failure");
            return -1;
        }
    }

    temp_buffer[20 - i] = '.';
    i++;
    if (quotient == 0) {
        temp_buffer[20 - i] = '0';
        i++;
    }
    while (quotient > 0) {
        temp_buffer[20 - i] = '0' + (quotient % 10);
        quotient = quotient / 10;
        i++;
        if (i > 20) {
            ESP_LOGI("DisplayFormatter", "Conversion Failure");
            return -1;
        }
    }

    if (i < 20) {
        for (int k = 0; k <= i; k++) {
            buffer[k + is_neg] = temp_buffer[20 - (i - 1) + k];
        }
    }
    return i + is_neg;
}

}  // namespace display
