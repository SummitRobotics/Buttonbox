#include <Arduino.h>
#include "Joystick.h"

Joystick_ joystick;

uint8_t data[2];

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    joystick.begin();
    DynamicHID().prepareOutput(data, sizeof(data));
}

// This function reads two bytes of output data from the joystick.
// Returns [0, 0xffff] on success
// Returning -1 if two bytes of output not available.
int32_t readJoystickOutput() {
    int available = DynamicHID().available();
    // No data, return -1.
    if (available <= 0) {
      return -1;
    }
    // Invalid number available bytes 
    if (available != 2) {
      while (available--) {
        DynamicHID().read();
        return -1;
      }
    }
    // Read BigEndian
    return ((int32_t)DynamicHID().read()) + (((int32_t)DynamicHID().read()) * 256);
}

void loop() {
    int32_t output = readJoystickOutput();
    if (output >= 0) {
        Serial.println(output);
        digitalWrite(LED_BUILTIN, output & 0x10);
    }
}
