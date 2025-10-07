#include "sensors.h"
#include "utilities.h"

float readTemperature() {
    float temp = -999.0;

    for (uint8_t i = 0; i < SENSOR_READ_RETRIES; i++) {
        tempSensor.requestTemperatures();
        temp = tempSensor.getTempCByIndex(0);

        if (temp != DEVICE_DISCONNECTED_C && temp != 85.0) {
            tempSensorOk = true;
            return temp;
        }
        delay(100);
    }

    Serial.println(F("FAULT: Temp sensor failed after retries."));
    tempSensorOk = false;
    return -999.0;
}

float readPh(float vcc) {
    int medianRaw = readAnalogMedian(PH_SENSOR_PIN, ANALOG_SAMPLE_COUNT);
    float voltage = medianRaw * vcc / 1024.0;

    float slope = (7.0 - 4.0) / (currentConfig.phNeutralVoltage - currentConfig.phAcidVoltage);
    float phValue = 7.0 + slope * (voltage - currentConfig.phNeutralVoltage);

    return isnan(phValue) ? -999.0 : phValue;
}

float readTurbidity(float vcc) {
    int medianRaw = readAnalogMedian(TURBIDITY_SENSOR_PIN, ANALOG_SAMPLE_COUNT);
    float voltage = medianRaw * vcc / 1024.0;
    return isnan(voltage) ? -999.0 : voltage;
}

float readVcc() {
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    delay(2);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));

    long result = ADCL | (ADCH << 8);

    if (result == 0) {
        return 0.0;
    }

    return 1.1 * 1023.0 / result;
}
