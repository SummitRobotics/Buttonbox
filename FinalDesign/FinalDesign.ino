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

#define FUN_MISSILE_LADDER A2

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

boolean isNear(int val, int ref, int epsilon = 4) {
  return abs(val - ref) <= epsilon;
}

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

// FUN_DIAL_LEFT + FUN_DIAL_RIGHT combination is not possible
int fun_missile_ladder_table[12] = {
    0,   // 0000
    318, // 0001
    505, // 0010
    602, // 0011
    176, // 0100
    407, // 0101
    555, // 0110
    635, // 0111
    88,  // 1000
    362, // 1001
    527, // 1010
    617, // 1011
};

int readLadderPin(int pin, int num_entries, int table[]) {
  int val = analogRead(pin);
  // Serial.println(val);
  for (int i = 0; i < num_entries; ++i) {
    if (isNear(val, table[i])) {
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

  bool button1 = (left_column_buttons & 0x1) != 0;
  bool button2 = (middle_column_buttons & 0x1) != 0;
  bool button3 = (right_column_buttons & 0x1) != 0;
  bool button4 = (left_column_buttons & 0x2) != 0;
  bool button5 = (middle_column_buttons & 0x2) != 0;
  bool button6 = (right_column_buttons & 0x2) != 0;
  bool button7 = (left_column_buttons & 0x4) != 0;
  bool button8 = (middle_column_buttons & 0x4) != 0;
  bool button9 = (right_column_buttons & 0x4) != 0;

  // Light up pins based on ladder bits set above
  digitalWrite(BUTTON_LED_1, button1 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_2, button2 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_3, button3 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_4, button4 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_5, button5 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_6, button6 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_7, button7 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_8, button8 ? HIGH : LOW);
  digitalWrite(BUTTON_LED_9, button9 ? HIGH : LOW);
 
  // Drive joystick buttons (0-based)
  Joystick.setButton(0, button1);
  Joystick.setButton(1, button2);
  Joystick.setButton(2, button3);
  Joystick.setButton(3, button4);
  Joystick.setButton(4, button5);
  Joystick.setButton(5, button6);
  Joystick.setButton(6, button7);
  Joystick.setButton(7, button8);
  Joystick.setButton(8, button9);

  // Read the fun dial and missile toggles
  int fun_missile_dial =
      readLadderPin(FUN_MISSILE_LADDER, 12, fun_missile_ladder_table);

  bool missileSwitch1 = (fun_missile_dial & MISSILE_SWITCH_1_MASK) != 0;
  bool missileSwitch2 = (fun_missile_dial & MISSILE_SWITCH_2_MASK) != 0;
  bool funDial1 = (fun_missile_dial & FUN_DIAL_LEFT_MASK) != 0;
  bool funDial3 = (fun_missile_dial & FUN_DIAL_RIGHT_MASK) != 0;
  bool funDial2 = !(funDial1 || funDial2);

  Joystick.setButton(9, missileSwitch1);
  Joystick.setButton(10, missileSwitch2);
  Joystick.setButton(11, funDial1);
  Joystick.setButton(12, funDial2);
  Joystick.setButton(13, funDial3);

  // Read and set encoder axis positions
  Joystick.setXAxisRotation(enc1.read());
  Joystick.setYAxisRotation(enc2.read());

  delay(50);
}
