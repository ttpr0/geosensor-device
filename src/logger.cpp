#include "./logger.h"
#include "./config.h"

#include <SD.h>

#ifdef LOGGING_SD_ENABLED
File log_file;
#endif

void logger_setup()
{
    Serial.begin(BAUD_RATE);
    delay(10000);  // wait for serial monitor
#ifdef LOGGING_SD_ENABLED
    if (!SD.begin(LOGGING_SD_PIN)) {
        Serial.println("initialization failed. Things to check:");
        while (true) {
            delay(10);
        }
    }
#endif
}

void logger_begin()
{
#ifdef LOGGING_SD_ENABLED
    log_file = SD.open(LOGGING_SD_FILE, FILE_WRITE);
    // check if the file opened okay
    if (!log_file) {
        Serial.println("error opening log-file");
    }
#endif
}

void logger_end()
{
#ifdef LOGGING_SD_ENABLED
    if (log_file) {
        log_file.close();
    }
#endif
}

void println()
{
    Serial.println();
#ifdef LOGGING_SD_ENABLED
    if (log_file) {
        log_file.println();
    }
#endif
}
void println(const String& message)
{
    Serial.println(message);
#ifdef LOGGING_SD_ENABLED
    if (log_file) {
        log_file.println(message);
    }
#endif
}
void print(const String& message)
{
    Serial.print(message);
#ifdef LOGGING_SD_ENABLED
    if (log_file) {
        log_file.print(message);
    }
#endif
}
void print(float value)
{
    Serial.print(value);
#ifdef LOGGING_SD_ENABLED
    if (log_file) {
        log_file.print(value);
    }
#endif
}
