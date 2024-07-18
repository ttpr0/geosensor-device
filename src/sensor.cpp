#include <SDI12.h>

#include "./config.h"
#include "./logger.h"
#include "./sensor.h"

SDI12 sdi_12(SDI12_DATA_PIN);
String command;  // global variable to reduce heap allocations
String sdi_response;

// keeps track of active addresses
bool is_active[64] = {0};
uint8_t num_sensors = 0;

// this checks for activity at a particular address
// expects a char, '0'-'9', 'a'-'z', or 'A'-'Z'
bool _check_active(char i)
{
    sdi_12.clearBuffer();
    // sends basic 'acknowledge' command [address][!]
    command = "";
    command += i;
    command += "!";
    // goes through three rapid contact attempts
    for (int j = 0; j < 3; j++) {
        sdi_12.sendCommand(command, SDI12_WAKE_DELAY);
        delay(100);
        if (sdi_12.available()) {
            sdi_12.clearBuffer();
            return true;
        }
    }
    sdi_12.clearBuffer();
    return false;
}

void _print_info(char i)
{
    sdi_12.clearBuffer();
    // send info command
    command = "";
    command += i;
    command += "I!";
    sdi_12.sendCommand(command, SDI12_WAKE_DELAY);
    // wait for response
    delay(100);
    while (sdi_12.available() < 3) {
        delay(10);
    }
    sdi_response = "";
    while (sdi_12.available()) {
        char c = sdi_12.read();
        if (c == '\n')
            break;
        sdi_response += c;
        delay(10);
    }
    sdi_response.trim();
    // print info
    print(sdi_response.substring(0, 1));  // address
    print(F(", "));
    print(sdi_response.substring(1, 3).toFloat() / 10);  // SDI-12 version number
    print(F(", "));
    print(sdi_response.substring(3, 11));  // vendor id
    print(F(", "));
    print(sdi_response.substring(11, 17));  // sensor model
    print(F(", "));
    print(sdi_response.substring(17, 20));  // sensor version
    print(F(", "));
    print(sdi_response.substring(20));  // sensor id
    println();
}

// convert address character to a decimal number
int8_t _char_to_dec(char i)
{
    if ((i >= '0') && (i <= '9'))
        return i - '0';
    if ((i >= 'a') && (i <= 'z'))
        return i - 'a' + 10;
    if ((i >= 'A') && (i <= 'Z'))
        return i - 'A' + 36;
    else
        return -1;
}

// convert a decimal number to an address character
char _dec_to_char(int8_t i)
{
    if (i > 0 && i < 10)
        return i + '0';
    if ((i >= 10) && (i < 36))
        return i + 'a' - 10;
    if ((i >= 36) && (i <= 62))
        return i + 'A' - 36;
    else
        return '@';
}

bool _take_measurement(char i, util::span<float>& results)
{
    sdi_12.clearBuffer();
    results.clear();
    // SDI-12 measurement command format  [address]['M'][!]
    command = "";
    command += i;
    command += "M!";
    sdi_12.sendCommand(command, SDI12_WAKE_DELAY);
    // wait for response
    while (sdi_12.available() < 3) {
        delay(10);
    }
    sdi_response = "";
    while (sdi_12.available()) {
        char c = sdi_12.read();
        if (c == '\n')
            break;
        sdi_response += c;
        delay(10);
    }
    sdi_response.trim();
    // extract information
    uint8_t wait = sdi_response.substring(1, 4).toInt();
    int num_results = sdi_response.substring(4).toInt();
    if (num_results == 0 || num_results > results.capacity()) {
        return false;
    }
    // wait for sensor to finish
    unsigned long timerStart = millis();
    while ((millis() - timerStart) < (1000UL * (wait + 1))) {
        if (sdi_12.available())  // sensor can interrupt us to let us know it is done early
        {
            sdi_12.clearBuffer();
            break;
        }
    }
    // read measurement results
    delay(30);
    sdi_12.clearBuffer();
    // SDI-12 command to get data [address][D][dataOption][!]
    command = "";
    command += i;
    command += "D0!";
    sdi_12.sendCommand(command, SDI12_WAKE_DELAY);
    // wait for response
    uint32_t timeout = 1500;
    uint32_t start = millis();
    while (sdi_12.available() < 3 && (millis() - start) < timeout) {}
    sdi_12.read();  // ignore the repeated SDI12 address
    if (sdi_12.peek() == '+') {
        sdi_12.read();
    }
    // read measurement results
    while (sdi_12.available()) {
        char c = sdi_12.peek();
        if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            float result = sdi_12.parseFloat(SKIP_NONE);
            if (result == -9999) {
                continue;
            }
            results.push_back(result);
        } else {
            sdi_12.read();
        }
        delay(10);  // 1 character ~ 7.5ms
    }
    sdi_12.clearBuffer();
    if (results.size() == num_results) {
        return true;
    } else {
        return false;
    }
}

void sensor_setup()
{
    command.reserve(20);
    sdi_response.reserve(50);
    sdi_12.begin();
    delay(500);  // allow things to settle
    // Power the sensors
    if (SDI12_POWER_PIN > 0) {
        pinMode(SDI12_POWER_PIN, OUTPUT);
        digitalWrite(SDI12_POWER_PIN, HIGH);
        delay(200);
    }

    // Scan the Address Space
    println("Scanning all addresses, please wait...");
    for (int8_t i = 0; i < 62; i++) {
        char addr = _dec_to_char(i);
        if (_check_active(addr)) {
            num_sensors++;
            is_active[i] = true;
            _print_info(addr);
        }
    }
    if (num_sensors > 0) {
        println("Total number of sensors found: " + String(num_sensors));
    } else {
        println("No sensors found, please check connections and restart the Arduino.");
        while (true) {
            delay(10);
        }
    }

    // Unpower the sensors
    if (SDI12_POWER_PIN > 0) {
        pinMode(SDI12_POWER_PIN, OUTPUT);
        digitalWrite(SDI12_POWER_PIN, LOW);
        delay(200);
    }
}

float measurement_buffer[MAX_MEASUREMENTS];
util::span<float> measurements(measurement_buffer, MAX_MEASUREMENTS);

bool sensor_measure(util::span<SensorResult>& results)
{
    results.clear();
    // Power the sensors
    if (SDI12_POWER_PIN > 0) {
        pinMode(SDI12_POWER_PIN, OUTPUT);
        digitalWrite(SDI12_POWER_PIN, HIGH);
        delay(200);
    }

    // measure one at a time
    for (int8_t i = 0; i < 62; i++) {
        char addr = _dec_to_char(i);
        if (is_active[i]) {
            measurements.clear();  // clear the measurement buffer
            bool success = _take_measurement(addr, measurements);
            if (!success || measurements.size() < 5) {
                println(F("No or invalid results received, please check connections and restart the Arduino."));
            } else {
                results.push_back({
                    .addr = addr,
                    .raw = (int)measurements[0],
                    .permittivity = measurements[1],
                    .water_content = measurements[2],
                    .temperature = measurements[3],
                    .supply_voltage = measurements[4],
                });
                if (results.size() == results.capacity()) {
                    break;
                }
            }
        }
    }

    // Unpower the sensors
    if (SDI12_POWER_PIN > 0) {
        pinMode(SDI12_POWER_PIN, OUTPUT);
        digitalWrite(SDI12_POWER_PIN, LOW);
        delay(200);
    }

    return true;
}
