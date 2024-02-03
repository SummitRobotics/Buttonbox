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

#define FUN_MISSLE_LADDER A2

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
int left_column_ladder_table[8] = {
    0,   // 000
    515, // 001
    616, // 010
    770, // 011
    682, // 100
    820, // 101
    829, // 110
    897  // 111
};

int middle_column_ladder_table[8] = {
    0,   // 000
    686, // 001
    617, // 010
    831, // 011
    512, // 100
    821, // 101
    770, // 110
    898  // 111
};

int right_column_ladder_table[8] = {
    0,   // 000
    685, // 001
    617, // 010
    831, // 011
    512, // 100
    821, // 101
    770, // 110
    897  // 111
};

#define MISSILE_SWITCH_1_MASK 0x1
#define MISSILE_SWITCH_2_MASK 0x2
#define FUN_DIAL_LEFT_MASK 0x4
#define FUN_DIAL_RIGHT_MASK 0x8

// FUN_DIAL_LEFT + FUN_DIAL_RIGHT not possible
int fun_missle_ladder_table[12] = {
    0,   // 0000
    407, // 0001
    555, // 0010
    635, // 0011
    176, // 0100
    407, // 0101
    505, // 0110
    602, // 0111
    88,  // 1000
    362, // 1001
    527, // 1010
    617, // 1011
};

int readLadderPin(int pin, int num_entries, int table[]) {
  int val = analogRead(pin);
  Serial.println(val);
  for (int i = 0; i < num_entries; ++i) {
    if (near(val, table[i])) {
      return i;
    }
  }
  return 0; // return default as nothing
}

void loop() {
  // Read button ladder pins
  int left_column_buttons =
      readLadderPin(LEFT_COLUMN_BUTTON_LADDER, 8, left_column_ladder_table);
  int middle_column_buttons =
      readLadderPin(MIDDLE_COLUMN_BUTTON_LADDER, 8, middle_column_ladder_table);
  int right_column_buttons =
      readLadderPin(RIGHT_COLUMN_BUTTON_LADDER, 8, right_column_ladder_table);


  // Light up pins based on ladder bits set above
  digitalWrite(BUTTON_LED_1, left_column_buttons & 0x1 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_2, middle_column_buttons & 0x1 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_3, right_column_buttons & 0x1 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_4, left_column_buttons & 0x2 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_5, middle_column_buttons & 0x2 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_6, right_column_buttons & 0x2 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_7, left_column_buttons & 0x4 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_8, middle_column_buttons & 0x4 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_9, right_column_buttons & 0x4 ? HIGH : LOW);

  // Read the fun dial and missle toggles
  int fun_missle_dial =
      readLadderPin(FUN_MISSLE_LADDER, 12, fun_missle_ladder_table);

  if (fun_missle_dial & MISSILE_SWITCH_1_MASK) {
    Serial.print("[Missle1] ");
  }
  if (fun_missle_dial & MISSILE_SWITCH_2_MASK) {
    Serial.print("[Missle2] ");
  }
  if (fun_missle_dial & FUN_DIAL_LEFT_MASK) {
    Serial.print("[FunDialLeft] ");
  }
  if (fun_missle_dial & FUN_DIAL_RIGHT_MASK) {
    Serial.print("[FunDialRight] ");
  }
  Serial.println("");

  // Joystick.setXAxisRotation(enc1.read());
  // Joystick.setYAxisRotation(enc2.read());
  // Joystick.setButton(0, ladder1 & 0x1);
  // Joystick.setButton(1, ladder1 & 0x2);

  delay(50);
}
