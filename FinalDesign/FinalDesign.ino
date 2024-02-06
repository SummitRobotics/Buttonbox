#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Encoder.h>
#include <Joystick.h>
#include <Wire.h>

// Encoders should use interrupt pins when possible [0-3, 7]
#define ENCODER_1_PIN_1 7
#define ENCODER_1_PIN_2 13
#define ENCODER_2_PIN_1 1
#define ENCODER_2_PIN_2 0

// [ 1 2 3 ]
// [ 4 5 6 ]
// [ 7 8 9 ]
#define BUTTON_LED_1 A0
#define BUTTON_LED_2 A1
#define BUTTON_LED_3 11
#define BUTTON_LED_4 6
#define BUTTON_LED_5 5
#define BUTTON_LED_6 4
#define BUTTON_LED_7 9
#define BUTTON_LED_8 8
#define BUTTON_LED_9 10

// [ 1 4 7 ]
#define LEFT_COLUMN_BUTTON_LADDER A3
// [ 2 5 8 ]
#define MIDDLE_COLUMN_BUTTON_LADDER A4
// [ 3 6 9 ]
#define RIGHT_COLUMN_BUTTON_LADDER A5

// [ MISSILE_SWITCH_1 MISSILE_SWITCH_2 FUN_DIAL_LEFT FUN_DIAL_RIGHT ]
#define FUN_MISSILE_LADDER A2

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // QT-PY / XIAO

Adafruit_SH1106G display =
    Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Joystick_ joystick;
uint8_t joystickData[2];

Encoder enc1(ENCODER_1_PIN_1, ENCODER_1_PIN_2);
Encoder enc2(ENCODER_2_PIN_1, ENCODER_2_PIN_2);

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

  // wait for the OLED to power up
  delay(250);
  display.begin(i2c_Address, true);
  display.display();

  Serial.begin(9600);
  joystick.begin(false);
  DynamicHID().prepareOutput(joystickData, sizeof(joystickData));
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

// FUN_DIAL_LEFT + FUN_DIAL_RIGHT combination is not possible, (11xx) bits.
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
  return 0;
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
  return ((int32_t)DynamicHID().read()) + (((int32_t)DynamicHID().read()) << 8);
}

// Upper two bits of the 16-bit driver station message indicate message type
enum DriverStationMessageType {
  MessageType_Mask = 0xC000, // up to 4 message types
  MessageType_Shift = 14,
  MessageType_ButtonLeds = 0,
  MessageType_ScreenMode = 1,
};

int circle1_r = 16;
int circle2_r = 16;

void handleDriverStationMessage(uint16_t message) {
  // Update lights based on the message from Driver Station
  uint16_t messageType = (message & MessageType_Mask) >> MessageType_Shift;
  if (messageType == MessageType_ButtonLeds) {
    digitalWrite(BUTTON_LED_1, bitRead(message, 0) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_2, bitRead(message, 1) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_3, bitRead(message, 2) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_4, bitRead(message, 3) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_5, bitRead(message, 4) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_6, bitRead(message, 5) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_7, bitRead(message, 6) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_8, bitRead(message, 7) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_9, bitRead(message, 8) ? HIGH : LOW);
  } else if (messageType == MessageType_ScreenMode) {
    circle1_r = 16;
    circle2_r = 16;
  }
}

void tick() {
  // Read button ladder pins
  int left_column_buttons =
      readLadderPin(LEFT_COLUMN_BUTTON_LADDER, 8, left_column_ladder_table);
  int middle_column_buttons =
      readLadderPin(MIDDLE_COLUMN_BUTTON_LADDER, 8, middle_column_ladder_table);
  int right_column_buttons =
      readLadderPin(RIGHT_COLUMN_BUTTON_LADDER, 8, right_column_ladder_table);

  // Compute boolean state of each button
  bool button1 = (left_column_buttons & 0x1) != 0;
  bool button2 = (middle_column_buttons & 0x1) != 0;
  bool button3 = (right_column_buttons & 0x1) != 0;
  bool button4 = (left_column_buttons & 0x2) != 0;
  bool button5 = (middle_column_buttons & 0x2) != 0;
  bool button6 = (right_column_buttons & 0x2) != 0;
  bool button7 = (left_column_buttons & 0x4) != 0;
  bool button8 = (middle_column_buttons & 0x4) != 0;
  bool button9 = (right_column_buttons & 0x4) != 0;

  // Drive joystick buttons (0-based)
  joystick.setButton(0, button1);
  joystick.setButton(1, button2);
  joystick.setButton(2, button3);
  joystick.setButton(3, button4);
  joystick.setButton(4, button5);
  joystick.setButton(5, button6);
  joystick.setButton(6, button7);
  joystick.setButton(7, button8);
  joystick.setButton(8, button9);

  // Read the fun dial and missile toggles
  int fun_missile_dial =
      readLadderPin(FUN_MISSILE_LADDER, 12, fun_missile_ladder_table);

  // Compute boolean state of each component. funDial2 is absences of funDial1
  // and funDial3
  bool missileSwitch1 = (fun_missile_dial & MISSILE_SWITCH_1_MASK) != 0;
  bool missileSwitch2 = (fun_missile_dial & MISSILE_SWITCH_2_MASK) != 0;
  bool funDial1 = (fun_missile_dial & FUN_DIAL_LEFT_MASK) != 0;
  bool funDial3 = (fun_missile_dial & FUN_DIAL_RIGHT_MASK) != 0;
  bool funDial2 = !(funDial1 || funDial3);

  // Drive states as buttons (0-based)
  joystick.setButton(9, missileSwitch1);
  joystick.setButton(10, missileSwitch2);
  joystick.setButton(11, funDial1);
  joystick.setButton(12, funDial2);
  joystick.setButton(13, funDial3);

  // Read encoders
  int enc1_val = enc1.readAndReset() / 4;
  int enc2_val = enc2.readAndReset() / 4;

  circle1_r = constrain(circle1_r + enc1_val, 1, 30);
  circle2_r = constrain(circle2_r + enc2_val, 1, 30);

  // Read the joystick output from the Driver Station
  int32_t dsMessage = readJoystickOutput();
  if (dsMessage >= 0) {
    handleDriverStationMessage(dsMessage);
  }

  display.clearDisplay();
  display.drawCircle(32, 32, circle1_r, SH110X_WHITE);
  display.drawCircle(96, 32, circle2_r, SH110X_WHITE);
  display.display();

  // Joystick.setXAxisRotation(enc1.read());
  // Joystick.setYAxisRotation(enc2.read());

  joystick.sendState();
}

#define TIME_LOOP 0

#if TIME_LOOP
unsigned long loopCount = 0;
unsigned long timeBegin = micros();
#endif

void loop() {
  tick();
#if TIME_LOOP
  loopCount++;
  unsigned long timeEnd = micros();
  unsigned long totalDuration = timeEnd - timeBegin;
  if (totalDuration >= 1000000) {
    Serial.print("Loops per second: ");
    Serial.print(loopCount);
    Serial.print(", time per loop: ");
    double timePerLoopMs = (totalDuration / 1000.0 / loopCount);
    Serial.print(timePerLoopMs);
    Serial.println("ms");
    loopCount = 0;
    timeBegin = timeEnd;
  }
#endif
}
