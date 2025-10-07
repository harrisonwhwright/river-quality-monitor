#include "power.h"

void goToSleep(long seconds) {
    long sleepCycles = seconds / 8;
    for (int i = 0; i < sleepCycles; i++) {
        wdt_reset();
        LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    }
}

void secureUnusedPins() {
    for (uint8_t i = 2; i < 20; ++i) {
        if (i == SD_CHIP_SELECT_PIN || i == TEMP_SENSOR_PIN ||
            i == SERVICE_MODE_PIN || i == SENSOR_POWER_PIN ||
            i == 10 || i == 11 || i == 12 || i == STATUS_LED_PIN ||
            i == PH_SENSOR_PIN || i == TURBIDITY_SENSOR_PIN ||
            i == 18 || i == 19) {
            continue;
        }
        pinMode(i, INPUT_PULLUP);
    }
}
