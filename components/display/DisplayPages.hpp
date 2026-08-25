#pragma once

#include <cstdint>

namespace display {

constexpr uint8_t kPageCount = 5;
constexpr uint8_t kRowCount = 4;
constexpr uint8_t kColumnCount = 20;
constexpr uint8_t kRowLayout[kRowCount] = {0, 2, 1, 3};

enum Page : uint8_t {
    kAlternatorPage = 0,
    kBatteryPage = 1,
    kHeaterStatusPage = 2,
    kPumpStatusPage = 3,
    kNetworkPage = 4,
};

void LoadDefaultPages(uint8_t pages[kPageCount][kRowCount][kColumnCount + 1]);

}  // namespace display
