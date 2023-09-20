// Simple example application that shows how to read four Arduino
// digital pins and map them to the USB Joystick library.
//
// The digital pins 9, 10, 11, and 12 are grounded when they are pressed.
//
// NOTE: This sketch file is for use with Arduino Leonardo and
//       Arduino Micro only.
//
// by Matthew Heironimus
// 2015-11-20
//--------------------------------------------------------------------

#include <Joystick.h>
#include <Encoder.h>

Encoder myEnc(3,4);

void setup() {
  // Initialize Button Pins
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  // Initialize Joystick Library
  Joystick.begin();



  Serial.begin(9600);
}

// Constant that maps the phyical pin to the joystick button.
const int pinToButtonMap = 6;

// Last state of the button
int lastButtonState[7] = {0,0,0,0};

void loop() {

  // Read pin values
  for (int index = 0; index < 7; index++)
  {
    int currentButtonState = !digitalRead(index + pinToButtonMap);
    if (currentButtonState != lastButtonState[index])
    {
      Joystick.setButton(index, currentButtonState);
      lastButtonState[index] = currentButtonState;
    }
  }

  int sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  Joystick.setThrottle(sensorValue/4);
  Joystick.setXAxisRotation(myEnc.read());

  Serial.println(digitalRead(3));
  Serial.println(digitalRead(4));

  Serial.println(myEnc.read());

  delay(50);
}

