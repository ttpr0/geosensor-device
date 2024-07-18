#pragma once

#include <Arduino.h>

void logger_setup();
void logger_begin();
void logger_end();

void println();
void println(const String& message);
void print(const String& message);
void print(float value);
