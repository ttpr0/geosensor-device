#pragma once

#include "./util.h"

// sensor result struct
struct SensorResult
{
    char addr;
    int raw;
    double permittivity;
    double water_content;
    double temperature;
    double supply_voltage;
};

// set up sensor
// this will look for active sensors on all sdi-12 adresses
// if a sensor wasnt found it will be ignored during measurement (all sensors need to be available during this call)
void sensor_setup();

// measure result
// returns true if successfull
// results are pushed to span
bool sensor_measure(util::span<SensorResult>& results);
