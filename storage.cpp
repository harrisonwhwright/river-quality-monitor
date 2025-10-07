#include "storage.h"

bool initializeSdCard() {
    Serial.print(F("Initializing SD card... "));
    if (!SD.begin(SD_CHIP_SELECT_PIN)) {
        Serial.println(F("FAILED"));
        return false;
    }
    Serial.println(F("OK"));
    return true;
}

void loadConfiguration() {
    Serial.print(F("Loading configuration from EEPROM... "));
    EEPROM.get(EEPROM_ADDR, currentConfig);

    if (currentConfig.eepromMagicNumber != EEPROM_MAGIC_NUMBER) {
        Serial.println(F("Invalid/No config found. Loading defaults."));
        currentConfig.eepromMagicNumber = EEPROM_MAGIC_NUMBER;
        currentConfig.phNeutralVoltage = 2.50;
        currentConfig.phAcidVoltage = 3.05;
        currentConfig.sleepIntervalSeconds = 300;
        saveConfiguration();
    } else {
        Serial.println(F("OK"));
    }
}

void saveConfiguration() {
    EEPROM.put(EEPROM_ADDR, currentConfig);
    Serial.println(F("Configuration saved to EEPROM."));
}

void logBootMessage() {
    if (sdCardOk) {
        dataFile = SD.open("datalog.csv", FILE_WRITE);
        if (dataFile) {
            if (dataFile.size() == 0) {
                dataFile.println(F("Date,Time,Vcc_V,Temp_C,pH,Turbidity_V"));
            }
            dataFile.println(F("--- SYSTEM BOOT ---"));
            dataFile.close();
        }
    }
}

void logToSdCard(const char* data) {
    if (sdCardOk) {
        dataFile = SD.open("datalog.csv", FILE_WRITE);
        if (dataFile) {
            dataFile.println(data);
            dataFile.close();
        } else {
            Serial.println(F("ERROR: Failed to open datalog.csv for writing!"));
            sdCardOk = false;
        }
    }
}
