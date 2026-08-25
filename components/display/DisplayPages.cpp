#include "DisplayPages.hpp"

namespace display {

namespace {

constexpr uint8_t kDefaultPages[kPageCount][kRowCount][kColumnCount + 1] = {
    {"Alt       AGM       ",
     "A         A         ",
     "V         V         ",
     "                    "},
    {"ALTERNATOR          ",
     "V                   ",
     "A                   ",
     "                    "},
    {"   Heater Status    ",
     "Diesel Heater       ",
     "Electric Heater     ",
     "Engine Heater       "},
    {"    Pump Status     ",
     "Water Pump          ",
     "Cabin Pump          ",
     "Engine Pump         "},
};

}  // namespace

void LoadDefaultPages(uint8_t pages[kPageCount][kRowCount][kColumnCount + 1]) {
    for (uint8_t page = 0; page < kPageCount; ++page) {
        for (uint8_t row = 0; row < kRowCount; ++row) {
            for (uint8_t col = 0; col < kColumnCount; ++col) {
                pages[page][row][col] = kDefaultPages[page][row][col];
            }
            pages[page][row][kColumnCount] = 0;
        }
    }
}

}  // namespace display
