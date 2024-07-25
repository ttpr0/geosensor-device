#include <Arduino.h>

#include "./config.h"
#include "./logger.h"
#include "./message.h"
#include "./sender.h"
#include "./sensor.h"
#include "./timer.h"

SensorResult sensor_buffer[MAX_SENSORS];
util::span<SensorResult> sensors(sensor_buffer, MAX_SENSORS);

#ifdef MESSAGE_BINARY_PAYLOAD
byte message_buffer[MESSAGE_MAX_PAYLOAD_SIZE];
util::buffer message(message_buffer, MESSAGE_MAX_PAYLOAD_SIZE);
#else
String message;
#endif

void setup()
{
    // setup serial logging
    logger_setup();
    logger_begin();  // start logging
    // setup status LED
    pinMode(LED_BUILTIN, OUTPUT);
    // start RTC module
    println(F("Starting RTC..."));
    rtc_setup();
    println(F("Found RTC"));
    // start LoRa module
    println(F("Starting LoRa..."));
    lora_setup();
    println(F("Successfully connected to the network"));
    // setup sd2-12 sensors
    println(F("Starting Sensors..."));
    sensor_setup();
    println(F("Successfully connected to the sensors"));
    // check if max payload size is enough for sending a single sensor-result
    if (MESSAGE_SENSOR_SIZE + MESSAGE_TIME_SIZE > MESSAGE_MAX_PAYLOAD_SIZE) {
        println(F("Maximum payload size exceeded, please increase the payload size by increasing the data-rate"));
        while (true) {
            delay(10);
        }
    }
#ifndef MESSAGE_BINARY_PAYLOAD
    message.reserve(MESSAGE_MAX_PAYLOAD_SIZE);
#endif
    logger_end();  // stop logging
}

#ifdef RESET_ON_FAILURE
#include "./reset.h"
static int unsuccessfull_count = 0;
#endif

void loop()
{
    // turn on status LED to indicate activity
    digitalWrite(LED_BUILTIN, HIGH);
    // start logging
    logger_begin();
    // keep track of progress
    bool success = true;
    // measure time
    DateTime now = rtc_now();
    auto _start = millis();
    // measure
    println(F("Measuring..."));
    sensors.clear();  // clear sensor buffer
    bool success_measure = sensor_measure(sensors);
    if (!success_measure) {
        println(F("No or invalid results received, please check connections and restart the Arduino"));
        success = false;
    } else {
        // build message
        // check if all results can be send in one message
        if (sensors.size() * MESSAGE_SENSOR_SIZE + MESSAGE_TIME_SIZE > MESSAGE_MAX_PAYLOAD_SIZE) {
            // send sensor messages individually
            for (auto sensor : sensors) {
                // encode message
#ifdef MESSAGE_BINARY_PAYLOAD
                message.clear();
                bool success_encode = message_binary(message, now, sensor);
#else
                message = "";
                bool success_encode = message_json(message, now, sensor);
#endif
                if (!success_encode) {
                    println(F("Failed to build message!"));
                    success = false;
                } else {
#ifndef MESSAGE_BINARY_PAYLOAD
                    println("Sending: " + message);
#endif
                    // send message
                    int err = lora_send(message);
                    if (err > 0) {
                        println(F("Message sent correctly!"));
                    } else {
                        println(F("Error sending message :("));
                        success = false;
                    }
                }
            }
        } else {
            // send all sensors in one message
            // encode message
#ifdef MESSAGE_BINARY_PAYLOAD
            message.clear();
            bool success_encode = message_binary(message, now, sensors);
#else
            message = "";
            bool success_encode = message_json(message, now, sensors);
#endif
            if (!success_encode) {
                println(F("Failed to build message!"));
                success = false;
            } else {
#ifndef MESSAGE_BINARY_PAYLOAD
                println("Sending: " + message);
#endif
                // send message
                int err = lora_send(message);
                if (err > 0) {
                    println(F("Message sent correctly!"));
                } else {
                    println(F("Error sending message :("));
                    success = false;
                }
            }
        }
    }
    // turn off status LED before going to sleep
    digitalWrite(LED_BUILTIN, LOW);
    // stop logging
    logger_end();
#ifdef RESET_ON_FAILURE
    // check if everything worked
    if (!success) {
        unsuccessfull_count += 1;
        if (unsuccessfull_count > FAIL_COUNT) {
            println(F("Restarting arduino..."));
            reset();
        }
    }
#endif
    // go to sleep
    auto _stop = millis();
    int _measure_seconds = (_stop - _start) / 1000;
    rtc_deep_sleep(SLEEP_DURATION_SECONDS - _measure_seconds);
}
