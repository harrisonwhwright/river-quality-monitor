#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

float readTemperature();

float readPh(float vcc);

float readTurbidity(float vcc);

float readVcc();

#endif
