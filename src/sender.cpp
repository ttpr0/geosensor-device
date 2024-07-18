#include "./sender.h"
#include "./config.h"

#ifdef LORA_SENDER_MKRWAN
#include <MKRWAN.h>

LoRaModem modem;
String appEui = LORA_SECRET_APP_EUI;
String appKey = LORA_SECRET_APP_KEY;
#else
#include <LoRa.h>
#include <SPI.h>
#endif

void lora_setup()
{
#ifdef LORA_SENDER_MKRWAN
    if (!modem.begin(EU868)) {
        Serial.println(F("Failed to start module"));
        while (true) {
            delay(10);
        }
    };
    if (!modem.dataRate(LORA_MKRWAN_DATARATE)) {
        Serial.println(F("Set data rate failed"));
        while (true) {
            delay(10);
        }
    }
    Serial.print(F("Your module version is: "));
    Serial.println(modem.version());
    Serial.print(F("Your device EUI is: "));
    Serial.println(modem.deviceEUI());
    int connected = modem.joinOTAA(appEui, appKey);
    if (!connected) {
        Serial.println(F("Something went wrong; are you indoor? Move near a window and retry"));
        while (true) {
            delay(10);
        }
    }
    modem.minPollInterval(60);
#else
    if (!LoRa.begin(868E6)) {
        Serial.println(F("Starting LoRa failed!"));
        while (true) {
            delay(10);
        }
    }
#endif
}

int lora_send(const String& msg)
{
    int err = 1;
#ifdef LORA_SENDER_MKRWAN
    modem.beginPacket();
    modem.write(msg.c_str(), msg.length());
    err = modem.endPacket(true);
#else
    LoRa.beginPacket();
    LoRa.write((uint8_t*)msg.c_str(), (size_t)msg.length());
    err = LoRa.endPacket();
#endif
    return err;
}

int lora_send(const util::buffer& msg)
{
    int err = 1;
#ifdef LORA_SENDER_MKRWAN
    modem.beginPacket();
    modem.write((uint8_t*)msg.data(), (size_t)msg.size());
    err = modem.endPacket(true);
#else
    LoRa.beginPacket();
    LoRa.write((uint8_t*)msg.data(), (size_t)msg.size());
    err = LoRa.endPacket();
#endif
    return err;
}
