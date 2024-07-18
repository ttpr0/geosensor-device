#pragma once

#include <RTClib.h>
#include <SPI.h>

// sets up the rtc module
void rtc_setup();

// go to deep-sleep for the given amount of seconds
void rtc_deep_sleep(int seconds);

// gets currents time from rtc-module
DateTime rtc_now();

// resets time of rtc module
void rtc_reset_time(const DateTime& now);
