/*
 * =============================================================================
 * River Medway Water Quality Monitor - v8.0 MODULAR
 * -----------------------------------------------------------------------------
 * RiverMedwayMonitor.ino
 *
 * This is the main sketch file. It acts as the orchestrator for the entire
 * system. Its responsibilities are to:
 * 1. Define the global objects and state variables.
 * 2. Initialize all modules in the correct order within setup().
 * 3. Execute the high-level operational logic within loop().
 * =============================================================================
 */

// --- MODULE INCLUDES ---
// Include the header file for each module. The config.h file is included
// by these headers, so it doesn't need to be included again here.
#include "power.h"
#include "storage.h"
#include "sensors.h"
#include "service_mode.h"
#include "utilities.h"

RTC_DS3231 rtc;
OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature tempSensor(&oneWire);
File dataFile;
Config currentConfig;

bool sdCardOk = false;
bool rtcOk = false;
bool tempSensorOk = false;

void setup() {
    wdt_disable();
    secureUnusedPins();

    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    pinMode(SERVICE_MODE_PIN, INPUT_PULLUP);
    digitalWrite(SENSOR_POWER_PIN, LOW);
    blinkLed(1, 200); // From utilities.cpp

    Serial.begin(9600);
    delay(2500);

    Serial.println(F("\n\n============================================="));
    Serial.println(F("  River Medway Monitor v8.0"));
    Serial.println(F("============================================="));

    loadConfiguration();

    rtcOk = initializeRtc();
    tempSensorOk = initializeTempSensor();
    sdCardOk = initializeSdCard();

    if (digitalRead(SERVICE_MODE_PIN) == LOW) {
        runServiceMode();
    }

    logBootMessage();

    Serial.println(F("Initialization complete. Entering normal operation."));
    wdt_enable(WDTO_8S);
    Serial.println(F("---------------------------------------------"));
    delay(100);
}

void loop() {
    wdt_reset();

    if (!sdCardOk) {
        Serial.println(F("STATE: SD Card fault. Attempting recovery..."));
        sdCardOk = initializeSdCard();
        if (sdCardOk) { logToSdCard("--- RECOVERY: SD Card re-initialized ---"); }
        else {
            Serial.println(F("Recovery failed. Entering extended sleep."));
            goToSleep(currentConfig.sleepIntervalSeconds * 2);
            return;
        }
    }
    if (!rtcOk) {
        Serial.println(F("STATE: RTC fault. Attempting recovery..."));
        rtcOk = initializeRtc();
        if (rtcOk) { logToSdCard("--- RECOVERY: RTC re-initialized ---"); }
    }
    if (!tempSensorOk) {
        Serial.println(F("STATE: Temp Sensor fault. Attempting recovery..."));
        tempSensorOk = initializeTempSensor();
        if (tempSensorOk) { logToSdCard("--- RECOVERY: Temp Sensor re-initialized ---"); }
    }

    Serial.println(F("\nWaking up to take readings..."));
    blinkLed(1, 50);

    digitalWrite(SENSOR_POWER_PIN, HIGH);
    Serial.println(F("Sensors powered ON. Stabilizing..."));
    delay(SENSOR_STABILIZE_MS);

    DateTime now = rtc.now();
    if (now.year() < CURRENT_YEAR) {
        logToSdCard("--- FAULT: RTC time invalid. Data logging paused. ---");
        rtcOk = false;
    } else {
        float vcc = readVcc();
        if (vcc < MINIMUM_VCC) {
            logToSdCard("--- FAULT: Vcc out of range. Check power source. ---");
        }

        float temperatureC = readTemperature();
        float phValue = readPh(vcc);
        float turbidityVoltage = readTurbidity(vcc);

        char dataString[128];
        snprintf(dataString, sizeof(dataString), "%04d/%02d/%02d,%02d:%02d:%02d,%.2f,%.2f,%.2f,%.2f",
                 now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(),
                 vcc, temperatureC, phValue, turbidityVoltage);

        logToSdCard(dataString);
        Serial.print(F("Logged: "));
        Serial.println(dataString);
    }

    digitalWrite(SENSOR_POWER_PIN, LOW);
    Serial.println(F("Sensors powered OFF."));

    Serial.print(F("Entering deep sleep for "));
    Serial.print(currentConfig.sleepIntervalSeconds);
    Serial.println(F(" seconds."));
    Serial.flush();
    goToSleep(currentConfig.sleepIntervalSeconds);
}
