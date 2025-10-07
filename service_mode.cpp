#include "service_mode.h"
#include "utilities.h"
#include "sensors.h"
#include "storage.h"

void runServiceMode() {
    Serial.println(F("\n*** SERVICE MODE ACTIVATED ***"));
    Serial.println(F("Type 'help' for commands."));
    wdt_disable();

    char cmdBuffer[64];
    char* command;

    while (true) {
        Serial.print(F("> "));
        if (readSerialLine(cmdBuffer, sizeof(cmdBuffer))) {
            command = strtok(cmdBuffer, " ");
            if (command != NULL) {
                handleSerialCommand(command);
            }
        }
    }
}

void handleSerialCommand(char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        Serial.println(F("Commands: status, settime, calibrate, interval, config, dump, format, factoryreset, reboot"));
    } else if (strcmp(cmd, "status") == 0) {
        digitalWrite(SENSOR_POWER_PIN, HIGH);
        delay(SENSOR_STABILIZE_MS);
        float vcc = readVcc();
        Serial.print(F("Vcc: ")); Serial.print(vcc); Serial.println(F(" V"));
        Serial.print(F("Temp: ")); Serial.print(readTemperature()); Serial.println(F(" C"));
        Serial.print(F("pH: ")); Serial.println(readPh(vcc));
        Serial.print(F("Turbidity: ")); Serial.print(readTurbidity(vcc)); Serial.println(F(" V"));
        digitalWrite(SENSOR_POWER_PIN, LOW);
    } else if (strcmp(cmd, "settime") == 0) {
        int y = atoi(strtok(NULL, " ")); int M = atoi(strtok(NULL, " ")); int d = atoi(strtok(NULL, " "));
        int h = atoi(strtok(NULL, " ")); int m = atoi(strtok(NULL, " ")); int s = atoi(strtok(NULL, " "));
        rtc.adjust(DateTime(y, M, d, h, m, s));
        Serial.println(F("Time updated."));
    } else if (strcmp(cmd, "calibrate") == 0) {
        char* arg1 = strtok(NULL, " ");
        if (arg1 != NULL && strcmp(arg1, "ph") == 0) {
            char* arg2 = strtok(NULL, " ");
            if (arg2 != NULL) {
                if (strcmp(arg2, "neutral") == 0) {
                    Serial.println(F("Place probe in pH 7 solution and wait 30s..."));
                    digitalWrite(SENSOR_POWER_PIN, HIGH); delay(30000);
                    float vcc = readVcc();
                    currentConfig.phNeutralVoltage = readAnalogMedian(PH_SENSOR_PIN, CALIBRATION_SAMPLE_COUNT) * vcc / 1024.0;
                    Serial.print(F("Neutral voltage captured: ")); Serial.println(currentConfig.phNeutralVoltage);
                    saveConfiguration();
                    digitalWrite(SENSOR_POWER_PIN, LOW);
                } else if (strcmp(arg2, "acid") == 0) {
                    Serial.println(F("Place probe in pH 4 solution and wait 30s..."));
                    digitalWrite(SENSOR_POWER_PIN, HIGH); delay(30000);
                    float vcc = readVcc();
                    currentConfig.phAcidVoltage = readAnalogMedian(PH_SENSOR_PIN, CALIBRATION_SAMPLE_COUNT) * vcc / 1024.0;
                    Serial.print(F("Acid voltage captured: ")); Serial.println(currentConfig.phAcidVoltage);
                    saveConfiguration();
                    digitalWrite(SENSOR_POWER_PIN, LOW);
                }
            }
        }
    } else if (strcmp(cmd, "interval") == 0) {
        char* arg = strtok(NULL, " ");
        if (arg != NULL) {
            currentConfig.sleepIntervalSeconds = atol(arg);
            saveConfiguration();
            Serial.print(F("Interval set to ")); Serial.print(currentConfig.sleepIntervalSeconds); Serial.println(F("s."));
        }
    } else if (strcmp(cmd, "config") == 0) {
        Serial.println(F("Current Configuration:"));
        Serial.print(F("  pH Neutral Voltage: ")); Serial.println(currentConfig.phNeutralVoltage);
        Serial.print(F("  pH Acid Voltage: ")); Serial.println(currentConfig.phAcidVoltage);
        Serial.print(F("  Sleep Interval: ")); Serial.print(currentConfig.sleepIntervalSeconds); Serial.println(F("s"));
    } else if (strcmp(cmd, "dump") == 0) {
        char* arg = strtok(NULL, " ");
        int linesToDump = (arg != NULL) ? atoi(arg) : 10;
        linesToDump = min(MAX_DUMP_LINES, linesToDump);
        Serial.print(F("Dumping last ")); Serial.print(linesToDump); Serial.println(F(" lines:"));
        dataFile = SD.open("datalog.csv", FILE_READ);
        if (dataFile) {
            char buffer[256]; int lineCount = 0;
            while(dataFile.available()) { dataFile.readBytesUntil('\n', buffer, sizeof(buffer)-1); lineCount++; }
            dataFile.seek(0);
            int startLine = max(0, lineCount - linesToDump);
            for(int i=0; i<startLine; i++) { dataFile.readBytesUntil('\n', buffer, sizeof(buffer)-1); }
            while(dataFile.available()) {
                int len = dataFile.readBytesUntil('\n', buffer, sizeof(buffer)-1); buffer[len] = '\0';
                Serial.println(buffer);
            }
            dataFile.close();
        } else { Serial.println(F("Could not open file.")); }
    } else if (strcmp(cmd, "format") == 0 || strcmp(cmd, "factoryreset") == 0) {
        Serial.println(F("This is a destructive action. Type 'CONFIRM' to proceed."));
        Serial.print(F("> "));
        char confirmBuf[10];
        readSerialLine(confirmBuf, sizeof(confirmBuf));
        if (strcmp(confirmBuf, "CONFIRM") == 0) {
            if (cmd[0] == 'f' && cmd[1] == 'o') {
                if (SD.remove("datalog.csv")) { Serial.println(F("File erased.")); }
                else { Serial.println(F("Could not erase file.")); }
            } else {
                currentConfig.eepromMagicNumber = 0; saveConfiguration();
                Serial.println(F("Settings reset. Rebooting..."));
                delay(1000); wdt_enable(WDTO_15MS); while(true);
            }
        } else { Serial.println(F("Action cancelled.")); }
    } else if (strcmp(cmd, "reboot") == 0) {
        Serial.println(F("Rebooting..."));
        delay(100);
        wdt_enable(WDTO_15MS); while(true);
    } else {
        Serial.println(F("Unknown command."));
    }
}
