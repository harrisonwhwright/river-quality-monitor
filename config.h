#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <avr/wdt.h>
#include "LowPower.h"
#include <EEPROM.h>

const uint8_t SD_CHIP_SELECT_PIN = 4;
const uint8_t TEMP_SENSOR_PIN = 7;
const uint8_t PH_SENSOR_PIN = A0;
const uint8_t TURBIDITY_SENSOR_PIN = A1;
const uint8_t STATUS_LED_PIN = 13;
const uint8_t SERVICE_MODE_PIN = 8;
const uint8_t SENSOR_POWER_PIN = 9;

const uint16_t SENSOR_STABILIZE_MS = 2000;
const uint8_t SENSOR_READ_RETRIES = 3;
const uint8_t ANALOG_SAMPLE_COUNT = 15;
const uint8_t CALIBRATION_SAMPLE_COUNT = 50;
const uint8_t MAX_DUMP_LINES = 50;
const uint16_t CURRENT_YEAR = 2025;
const float MINIMUM_VCC = 2.7;

struct Config {
    long eepromMagicNumber;
    float phNeutralVoltage;
    float phAcidVoltage;
    long sleepIntervalSeconds;
};

extern RTC_DS3231 rtc;
extern OneWire oneWire;
extern DallasTemperature tempSensor;
extern File dataFile;
extern Config currentConfig;

extern bool sdCardOk;
extern bool rtcOk;
extern bool tempSensorOk;

#endif
