#include <Arduino.h>

#include "./config.h"
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
    Serial.begin(BAUD_RATE);
    delay(10000);  // wait for serial monitor
    // start RTC module
    Serial.println(F("Starting RTC..."));
    rtc_setup();
    Serial.println(F("Found RTC"));
    // start LoRa module
    Serial.println(F("Starting LoRa..."));
    lora_setup();
    Serial.println(F("Successfully connected to the network"));
    // setup sd2-12 sensors
    Serial.println(F("Starting Sensors..."));
    sensor_setup();
    Serial.println(F("Successfully connected to the sensors"));
    // check if max payload size is enough for sending a single sensor-result
    if (MESSAGE_SENSOR_SIZE + MESSAGE_TIME_SIZE > MESSAGE_MAX_PAYLOAD_SIZE) {
        Serial.println(F("Maximum payload size exceeded, please increase the payload size by increasing the data-rate"));
        while (true) {
            delay(10);
        }
    }
#ifndef MESSAGE_BINARY_PAYLOAD
    message.reserve(MESSAGE_MAX_PAYLOAD_SIZE);
#endif
}

void loop()
{
    // measure time
    DateTime now = rtc_now();
    auto _start = millis();
    // measure
    Serial.println(F("Measuring..."));
    sensors.clear();  // clear sensor buffer
    bool success = sensor_measure(sensors);
    if (!success) {
        Serial.println(F("No or invalid results received, please check connections and restart the Arduino"));
    } else {
        // build message
        // check if all results can be send in one message
        if (sensors.size() * MESSAGE_SENSOR_SIZE + MESSAGE_TIME_SIZE > MESSAGE_MAX_PAYLOAD_SIZE) {
            // send sensor messages individually
            for (auto sensor : sensors) {
                // encode message
#ifdef MESSAGE_BINARY_PAYLOAD
                message.clear();
                success = message_binary(message, now, sensor);
#else
                message = "";
                success = message_json(message, now, sensor);
#endif
                if (!success) {
                    Serial.println(F("Failed to build message!"));
                } else {
#ifndef MESSAGE_BINARY_PAYLOAD
                    Serial.println("Sending: " + message);
#endif
                    // send message
                    int err = lora_send(message);
                    if (err > 0) {
                        Serial.println(F("Message sent correctly!"));
                    } else {
                        Serial.println(F("Error sending message :("));
                    }
                }
            }
        } else {
            // send all sensors in one message
            // encode message
#ifdef MESSAGE_BINARY_PAYLOAD
            message.clear();
            success = message_binary(message, now, sensors);
#else
            message = "";
            success = message_json(message, now, sensors);
#endif
            if (!success) {
                Serial.println(F("Failed to build message!"));
            } else {
#ifndef MESSAGE_BINARY_PAYLOAD
                Serial.println("Sending: " + message);
#endif
                // send message
                int err = lora_send(message);
                if (err > 0) {
                    Serial.println(F("Message sent correctly!"));
                } else {
                    Serial.println(F("Error sending message :("));
                }
            }
        }
    }
    // go to sleep
    auto _stop = millis();
    int _measure_seconds = (_stop - _start) / 1000;
    rtc_deep_sleep(SLEEP_DURATION_SECONDS - _measure_seconds);
}
