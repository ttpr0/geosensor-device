#pragma once

#include <Sodaq_wdt.h>

// reset arduino using watchdog timer
void reset()
{
    while (true) {
        sodaq_wdt_enable(WDT_PERIOD_1DIV64);
        for (;;) {};
    }
}
