#include <Joystick.h>
#include <Encoder.h>

const int encoder_pin1 = 3;
const int encoder_pin2 = 4;
// const int tri_button_pin_0 = 5;
const int push_button_pin_offset = 6;

const int potentiometer_pin = A0;

const int num_buttons = 15;

Encoder myEnc(encoder_pin1, encoder_pin2);


void setup() {
  // Initialize Button Pins
  // pinMode(tri_button_pin_0, INPUT_PULLUP);
  // pinMode(tri_button_pin_1, INPUT_PULLUP);
  // pinMode(tri_button_pin_2, INPUT_PULLUP);

  for (int i = 0; i < num_buttons; ++i) {
    pinMode(push_button_pin_offset + i, INPUT_PULLUP);
  }
  // Initialize Joystick Library
  Joystick.begin();

  Serial.begin(9600);
}

// Last state of the button
int lastButtonState[num_buttons] = {};

void loop() {

  // Read pin values
  for (int index = 0; index < num_buttons; index++)
  {
    int currentButtonState = !digitalRead(index + push_button_pin_offset);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  int sensorValue = analogRead(potentiometer_pin);
  Joystick.setThrottle(sensorValue/4);
  Joystick.setXAxisRotation(myEnc.read());

  // Serial.println(sensorValue);

  // Serial.println(digitalRead(3));
  // Serial.println(digitalRead(4));

  // Serial.println(myEnc.read());

  delay(50);
}