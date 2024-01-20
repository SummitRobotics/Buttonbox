// #include <Joystick.h>
// #include <Encoder.h>

// const int encoder_pin1 = 3;
// const int encoder_pin2 = 4;
// // const int tri_button_pin_0 = 5;
// const int push_button_pin_offset = 6;

// const int potentiometer_pin = A0;

// const int num_buttons = 15;
// Encoder myEnc(encoder_pin1, encoder_pin2);



// void setup() {
//   // Initialize Button Pins
//   // pinMode(tri_button_pin_0, INPUT_PULLUP);
//   // pinMode(tri_button_pin_1, INPUT_PULLUP);
//   // pinMode(tri_button_pin_2, INPUT_PULLUP);

//   for (int i = 0; i < num_buttons; ++i) {
//     pinMode(push_button_pin_offset + i, INPUT_PULLUP);
//   }
//   // Initialize Joystick Library
//   Joystick.begin();

//   Serial.begin(9600);
// }

// // Last state of the button
// int lastButtonState[num_buttons] = {};

// void loop() {

//   // Read pin values
//   for (int index = 0; index < num_buttons; index++)
//   {
//     int currentButtonState = !digitalRead(index + push_button_pin_offset);
//     if (currentButtonState != lastButtonState[index])
//     {
//       Joystick.setButton(index, currentButtonState);
//       lastButtonState[index] = currentButtonState;
//     }
//   }

//   int sensorValue = analogRead(potentiometer_pin);
//   // Joystick.setThrottle(sensorValue/4);
//   Joystick.setXAxisRotation(myEnc.read());

//   // Serial.println(sensorValue);

//   // Serial.println(digitalRead(3));
//   // Serial.println(digitalRead(4));

//   // Serial.println(myEnc.read());

//   delay(50);
// }
#include <Joystick.h>
#include <Encoder.h>

Encoder enc1(12, 13);
Encoder enc2(11, 10);


void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  Serial.begin(9600);
  Joystick.begin();
}

// button values will most likely be different this is just a template
int near(int val, int ref, int dist = 2) {
  return abs(val - ref) <= dist;
}

// int near2(int val2, int ref, int dist = 2) {
//   return abs(val2 - ref) <= dist;
// }

/*
 * This function reads the given analog pin and converts to a 3-bit digital value.
 */
int readLadderPin(int pin) {
  int val = analogRead(pin);
  Serial.println(val);
  if (near(val, 685)) return 1;
  if (near(val, 617)) return 2;
  if (near(val, 821)) return 3;
  if (near(val, 512)) return 4;
  if (near(val, 831)) return 5;
  if (near(val, 770)) return 6;
  if (near(val, 897)) return 7;
  return 0;
}


void loop() {

  // int val2 = analogRead(A2);
  // if (near(val2, 176))  {
  // digitalWrite(2, HIGH);
  // digitalWrite(4, HIGH);
  // digitalWrite(5, HIGH);
  // digitalWrite(6, HIGH);
  // digitalWrite(7, HIGH);
  // digitalWrite(8, HIGH);
  // digitalWrite(9, HIGH);
  // digitalWrite(A0, HIGH);
  // digitalWrite(A1, HIGH);
  // } 
  // else {
  //   digitalWrite(2, LOW);
  //   digitalWrite(4, LOW);
  //   digitalWrite(5, LOW);
  //   digitalWrite(6, LOW);
  //   digitalWrite(7, LOW);
  //   digitalWrite(8, LOW);
  //   digitalWrite(9, LOW);
  //   digitalWrite(A0, LOW);
  //   digitalWrite(A1, LOW);
  // }


  Joystick.setXAxisRotation(enc1.read());
  Joystick.setYAxisRotation(enc2.read());

  int camel = analogRead(A2);
  Serial.println(camel);

  int ladder1 = readLadderPin(A3);
  Joystick.setButton(0, ladder1 & 0x1);
  Joystick.setButton(1, ladder1 & 0x2);
  Joystick.setButton(2, ladder1 & 0x4);

  int ladder2 = readLadderPin(A4);
  Joystick.setButton(3, ladder2 & 0x1);
  Joystick.setButton(4, ladder2 & 0x2);
  Joystick.setButton(5, ladder2 & 0x4);

  int ladder3 = readLadderPin(A5);
  Joystick.setButton(6, ladder3 & 0x1);
  Joystick.setButton(7, ladder3 & 0x2);
  Joystick.setButton(8, ladder3 & 0x4);

  // delay(50);
}