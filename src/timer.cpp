#include <ArduinoLowPower.h>

#include "./config.h"
#include "./timer.h"

RTC_DS3231 rtc;

void rtc_reset_time(const DateTime& now)
{
    rtc.adjust(now);
}

DateTime rtc_now()
{
    return rtc.now();
}

void _on_rtc_alarm()
{
    if (rtc.alarmFired(1)) {
        rtc.clearAlarm(1);
    }
}

void rtc_setup()
{
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (true) {
            delay(10);
        }
    }
    rtc.disable32K();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
    rtc.disableAlarm(2);
    // set interrupt handler for RTC alarm
    pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);
}

void rtc_deep_sleep(int seconds)
{
#ifdef RTC_DEEP_SLEEP
    // if (!rtc.setAlarm1(rtc.now() + TimeSpan(seconds), DS3231_A1_Second)) {
    //     delay(seconds * 1000);  // fall back to non-deep sleep
    // }
    // LowPower.attachInterruptWakeup(RTC_INTERRUPT_PIN, _on_rtc_alarm, FALLING);
    // LowPower.deepSleep();
    LowPower.sleep(seconds * 1000);
#else
    delay(seconds * 1000);
#endif
}
