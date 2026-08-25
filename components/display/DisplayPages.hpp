#pragma once

#include <cstdint>

namespace display {

constexpr uint8_t kPageCount = 6;
constexpr uint8_t kRowCount = 4;
constexpr uint8_t kColumnCount = 20;
constexpr uint8_t kRowLayout[kRowCount] = {0, 2, 1, 3};

enum Page : uint8_t {
    kAlternatorPage = 0,
    kPassengerBatteryPage = 1,
    kDriverBatteryPage = 2,
    kHeaterStatusPage = 3,
    kPumpStatusPage = 4,
    kNetworkPage = 5,
};

void LoadDefaultPages(uint8_t pages[kPageCount][kRowCount][kColumnCount + 1]);

}  // namespace display
