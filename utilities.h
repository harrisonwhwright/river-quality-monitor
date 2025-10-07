#ifndef UTILITIES_H
#define UTILITIES_H

#include "config.h"

void blinkLed(int count, int duration);

int readAnalogMedian(int pin, int samples);

bool readSerialLine(char* buffer, int bufferSize);

#endif
