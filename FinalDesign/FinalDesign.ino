#include <Encoder.h>
#include <Joystick.h>


// Pin Layout
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

Encoder enc1(ENCODER_1_PIN_1, ENCODER_1_PIN_2);
Encoder enc2(ENCODER_2_PIN_1, ENCODER_2_PIN_1);

void setup() {
  // Initialize all output pins and drive LOW.
  pinMode(BUTTON_LED_1, OUTPUT);
  pinMode(BUTTON_LED_2, OUTPUT);
  pinMode(BUTTON_LED_3, OUTPUT);
  pinMode(BUTTON_LED_4, OUTPUT);
  pinMode(BUTTON_LED_5, OUTPUT);
  pinMode(BUTTON_LED_6, OUTPUT);
  pinMode(BUTTON_LED_7, OUTPUT);
  pinMode(BUTTON_LED_8, OUTPUT);
  pinMode(BUTTON_LED_9, OUTPUT);

  digitalWrite(BUTTON_LED_1, LOW);
  digitalWrite(BUTTON_LED_2, LOW);
  digitalWrite(BUTTON_LED_3, LOW);
  digitalWrite(BUTTON_LED_4, LOW);
  digitalWrite(BUTTON_LED_5, LOW);
  digitalWrite(BUTTON_LED_6, LOW);
  digitalWrite(BUTTON_LED_7, LOW);
  digitalWrite(BUTTON_LED_8, LOW);
  digitalWrite(BUTTON_LED_9, LOW);

  Serial.begin(9600);
  Joystick.begin();
}

// button values will most likely be different this is just a template
int near(int val, int ref, int dist = 4) { return abs(val - ref) <= dist; }

// int near2(int val2, int ref, int dist = 2) {
//   return abs(val2 - ref) <= dist;
// }

/*
 * This function reads the given analog pin and converts to a 3-bit digital
 * value.
 */
int left_row_ladder_table[8] = { 
  0,    // 000
  515,  // 001
  616,  // 010
  770,  // 011
  682,  // 100
  820,  // 101
  829,  // 110
  897   // 111
};

int middle_row_ladder_table[8] = { 
  0,    // 000
  686,  // 001
  617,  // 010
  831,  // 011
  512,  // 100
  821,  // 101
  770,  // 110
  898   // 111
};

int right_row_ladder_table[8] = { 
  0,    // 000
  685,  // 001
  617,  // 010
  831,  // 011
  512,  // 100
  821,  // 101
  770,  // 110
  897   // 111
};

int readLadderPin(int pin, int num_entries, int table[]) {
  int val = analogRead(pin);
  for (int i = 0; i < num_entries; ++i) {
    if (near(val, table[i])) {
      return i;
    }
  }
  return 0; // return default as nothing
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
  Serial.println(
      readLadderPin(LEFT_COLUMN_BUTTON_LADDER, 8, left_row_ladder_table));

  // int ladder1 = readLadderPin(A3);
  // Joystick.setButton(0, ladder1 & 0x1);
  // Joystick.setButton(1, ladder1 & 0x2);
  // Joystick.setButton(2, ladder1 & 0x4);

  // int ladder2 = readLadderPin(A4);
  // Joystick.setButton(3, ladder2 & 0x1);
  // Joystick.setButton(4, ladder2 & 0x2);
  // Joystick.setButton(5, ladder2 & 0x4);

  // int ladder3 = readLadderPin(A5);
  // Joystick.setButton(6, ladder3 & 0x1);
  // Joystick.setButton(7, ladder3 & 0x2);
  // Joystick.setButton(8, ladder3 & 0x4);

  delay(50);
}
