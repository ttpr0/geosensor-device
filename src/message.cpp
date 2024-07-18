#include <ArduinoJson.h>

#include "./config.h"
#include "./message.h"

String sensor_key = "s";
String time_key = "t";
String address_key = "a";
String raw_key = "r";
String permittivity_key = "p";
String water_content_key = "w";
String temperature_key = "t";
String supply_voltage_key = "s";

StaticJsonDocument<1024> doc;

template <typename T>
T round2(T value)
{
    return (int)(value * 100 + 0.5) / 100.0;
}

template <typename T>
T round3(T value)
{
    return (int)(value * 1000 + 0.5) / 1000.0;
}

bool message_json(String& msg, const DateTime& time, const util::span<SensorResult>& results)
{
    doc.clear();
    for (int i = 0; i < results.size(); i++) {
        address_key[0] = results[i].addr;
        // doc[sensor_key][address_key][permittivity_key] = round3(results[i].permittivity);
        doc[sensor_key][address_key][water_content_key] = round2(results[i].water_content);
        doc[sensor_key][address_key][temperature_key] = round2(results[i].temperature);
    }
    doc[time_key] = String(time.hour()) + ":" + String(time.minute()) + ":" + String(time.second());
    serializeJson(doc, msg);
    return true;
}

bool message_json(String& msg, const DateTime& time, const SensorResult& result)
{
    doc.clear();
    address_key[0] = result.addr;
    // doc[sensor_key][address_key][permittivity_key] = round3(result.permittivity);
    doc[sensor_key][address_key][water_content_key] = round2(result.water_content);
    doc[sensor_key][address_key][temperature_key] = round2(result.temperature);
    doc[time_key] = String(time.hour()) + ":" + String(time.minute()) + ":" + String(time.second());
    serializeJson(doc, msg);
    return true;
}

bool message_binary(util::buffer& msg, const DateTime& time, const util::span<SensorResult>& results)
{
    msg.write_time(time);
    for (int i = 0; i < results.size(); i++) {
        msg.write_char(results[i].addr);
        // msg.write_float(results[i].permittivity);
        msg.write_float(results[i].water_content);
        msg.write_float(results[i].temperature);
    }
    return true;
}

bool message_binary(util::buffer& msg, const DateTime& time, const SensorResult& result)
{
    msg.write_time(time);
    msg.write_char(result.addr);
    // msg.write_float(result.permittivity);
    msg.write_float(result.water_content);
    msg.write_float(result.temperature);
    return true;
}
