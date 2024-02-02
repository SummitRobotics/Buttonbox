#include <Joystick.h>
#include <Encoder.h>


#define ENCODER_1_PIN_1 3
#define ENCODER_1_PIN_2 2
#define ENCODER_2_PIN_1 1
#define ENCODER_2_PIN_2 0

#define BUTTON_LED_1 A0
#define BUTTON_LED_2 A1
#define BUTTON_LED_3 10
#define BUTTON_LED_4 6
#define BUTTON_LED_5 5
#define BUTTON_LED_6 4
#define BUTTON_LED_7 9
#define BUTTON_LED_8 8
#define BUTTON_LED_9 7

#define LEFT_COLUMN_BUTTON_LADDER A3
#define MIDDLE_COLUMN_BUTTON_LADDER A4
#define RIGHT_COLUMN_BUTTON_LADDER A5

#define FUN_DIAL_LADDER A2

#define MISSILE_SWITCH_1 13
#define MISSILE_SWITCH_2 A1



Encoder enc1(3, 2);
Encoder enc2(1, 0);


void setup() {
  // pinMode(2, OUTPUT);
  // digitalWrite(2, LOW);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
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
  digitalWrite(A1, LOW);
  pinMode(A0, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A1, OUTPUT);

  Serial.begin(9600);
  Joystick.begin();

    digitalWrite(10, HIGH);
}

// button values will most likely be different this is just a template
int near(int val, int ref, int dist = 4) {
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
  // Serial.println(val);
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
    // digitalWrite(2, LOW);
    // digitalWrite(3, LOW);
    // digitalWrite(4, LOW);
    // digitalWrite(5, LOW);
    // digitalWrite(6, LOW);
    // digitalWrite(7, LOW);
    // digitalWrite(8, LOW);
    // digitalWrite(9, LOW);
    // digitalWrite(A0, LOW);
    // digitalWrite(A1, LOW);
    // digitalWrite(A2, LOW);
    // digitalWrite(A3, LOW);
    // digitalWrite(A4, LOW);
    // digitalWrite(A5, LOW);
  // }


  // Joystick.setXAxisRotation(enc1.read());
  // Joystick.setYAxisRotation(enc2.read());

  Serial.println(enc2.read());
  // Serial.println(enc2.read());

  // int camel = analogRead(A2);
  // Serial.println(camel);

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

