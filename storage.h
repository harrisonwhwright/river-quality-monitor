#ifndef STORAGE_H
#define STORAGE_H

#include "config.h"

bool initializeSdCard();

void loadConfiguration();

void saveConfiguration();

void logBootMessage();

void logToSdCard(const char* data);

#endif
