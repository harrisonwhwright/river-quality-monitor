#include "utilities.h"

void blinkLed(int count, int duration) {
    for (int i = 0; i < count; i++) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(duration / 2);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(duration / 2);
    }
}

int readAnalogMedian(int pin, int samples) {
    const int MAX_SAMPLES = 50;
    if (samples > MAX_SAMPLES) {
        samples = MAX_SAMPLES;
    }
    int values[MAX_SAMPLES];

    for (int i = 0; i < samples; i++) {
        values[i] = analogRead(pin);
        delay(10);
    }

    for (int i = 0; i < samples - 1; i++) {
        for (int j = 0; j < samples - i - 1; j++) {
            if (values[j] > values[j + 1]) {
                int temp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = temp;
            }
        }
    }
    return values[samples / 2];
}

bool readSerialLine(char* buffer, int bufferSize) {
    int bytesRead = Serial.readBytesUntil('\n', buffer, bufferSize - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        if (buffer[bytesRead - 1] == '\r') {
            buffer[bytesRead - 1] = '\0';
        }
        return true;
    }
    return false;
}
