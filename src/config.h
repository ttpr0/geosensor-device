#pragma once

#define BAUD_RATE 115200
#define SLEEP_DURATION_SECONDS 30  // sleep duration between measurements
#define MAX_MEASUREMENTS 10        // maximum number of measurements (smt100 makes 5)
#define MAX_SENSORS 10             // maximum number of sensors to use

#define LORA_SECRET_APP_EUI "68f12b93c190a8d9"                  // lora application id
#define LORA_SECRET_APP_KEY "ebf1f5bffef66e282105140de1d3ea67"  // lora application key
#define LORA_SENDER_MKRWAN                                      // if defined mkrwan will be used, LoRa instead
#define LORA_MKRWAN_DATARATE 4                                  // datarate of lora transmission

#define RTC_INTERRUPT_PIN 1
#define RTC_DEEP_SLEEP

#define SDI12_DATA_PIN 7    // SDI-12 data pin
#define SDI12_POWER_PIN -1  // sensor power pin (or -1)
#define SDI12_WAKE_DELAY 0  // extra time needed for the sensor to wake (0-100ms)

#define MESSAGE_MAX_PAYLOAD_SIZE 64  // directly related to the LoRa datarate
#define MESSAGE_BINARY_PAYLOAD       // use binary payload instead of json to reduce payload size
#ifdef MESSAGE_BINARY_PAYLOAD
#define MESSAGE_SENSOR_SIZE 9
#define MESSAGE_TIME_SIZE 7
#else
#define MESSAGE_SENSOR_SIZE 35
#define MESSAGE_TIME_SIZE 23
#endif