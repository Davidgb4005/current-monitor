#include "stdint.h"

#define PAGES 4
#define ROWS 4
#define COLS 20

#if ROWS == 4
static const uint8_t ROW_LAYOUT[] = {0, 2, 1, 3};
#elif ROWS == 2
static const uint8_t ROW_LAYOUT[] = {0, 1};
#endif

static const uint8_t default_display[PAGES][ROWS][COLS + 1] = {
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
     "Engine Pump         "}};
