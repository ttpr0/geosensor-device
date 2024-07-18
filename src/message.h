#pragma once

#include <Arduino.h>
#include <RTClib.h>

#include "./sensor.h"
#include "./util.h"

// encode sensor results as json and write to msg
bool message_json(String& msg, const DateTime& time, const util::span<SensorResult>& results);
// encode sensor result as json and write to msg
bool message_json(String& msg, const DateTime& time, const SensorResult& result);

// encode sensor results as binary message and write to buffer
bool message_binary(util::buffer& msg, const DateTime& time, const util::span<SensorResult>& results);
// encode sensor result as binary message and write to buffer
bool message_binary(util::buffer& msg, const DateTime& time, const SensorResult& result);
