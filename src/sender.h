#pragma once

#include <Arduino.h>

#include "./util.h"

// setups up connection
void lora_setup();

// send the message
// returns an error code (if > 0 -> success)
int lora_send(const String& msg);
// send the message buffer
// returns an error code (if > 0 -> success)
int lora_send(const util::buffer& msg);
