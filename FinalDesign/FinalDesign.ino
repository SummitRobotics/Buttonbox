#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Encoder.h>
#include <Joystick.h>
#include <Wire.h>
// #include <FastTrig.h>

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
#define COLOR_WHITE SH110X_WHITE
#define COLOR_BLACK SH110X_BLACK

// Uncomment if display is inverted
// #define IMAGES_INVERTED 

#ifndef IMAGES_INVERTED
#include "images/BUTTERFLY_UP.h"
const unsigned char *startup_image = BUTTERFLY_UP_bmp;
#else
#include "images/BUTTERFLY_DOWN.h"
const unsigned char *startup_image = BUTTERFLY_DOWN_bmp;
#endif

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
  display.clearDisplay();
  display.display();

  Serial.begin(9600);
  joystick.begin(false);
  DynamicHID().prepareOutput(joystickData, sizeof(joystickData));

  display.drawXBitmap(0, 0, startup_image, 128, 64, COLOR_WHITE);
  display.display();
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

// Only have 16-bit messages to work with:
enum MessageEnums {
  // [14..15]: MessageType
  MessageType_Mask = 0xC000, // up to 4 message types
  MessageType_Shift = 14,
  MessageType_LedsUpdate = 0,
  MessageType_ViewUpdate = 1,
  // [12..13]: ViewType
  ViewType_Mask = 0x3000, // up to 4 view types
  ViewType_Shift = 12,
  ViewType_None = 0,
  ViewType_Progress = 1,
  ViewType_Orientation = 2,
  ViewType_Counter = 3,
  // [11]: Frame
  Frame_Mask = 0x800,
  Frame_Shift = 11,
  // [10]: Value
  Value_Mask = 0x7ff,
  Value_Shift = 0
};

uint16_t GET_MESSAGE_TYPE(uint16_t val) {
  return (val & MessageType_Mask) >> MessageType_Shift;
}
uint16_t SET_MESSAGE_TYPE(uint16_t val) { return val << MessageType_Shift; }
uint16_t GET_VIEW_TYPE(uint16_t val) {
  return (val & ViewType_Mask) >> ViewType_Shift;
}
uint16_t SET_VIEW_TYPE(uint16_t val) { return val << ViewType_Shift; }
uint16_t GET_FRAME(uint16_t val) { return (val & Frame_Mask) >> Frame_Shift; }
uint16_t SET_FRAME(uint16_t val) { return val << Frame_Shift; }
uint16_t GET_VALUE(uint16_t val) { return (val & Value_Mask) >> Value_Shift; }
uint16_t SET_VALUE(uint16_t val) { return val << Value_Shift; }

struct Vertex2i {
  int16_t x;
  int16_t y;
};

struct DrawState {
  uint8_t view;  // View type
  int16_t value; // ViewType_Orientation - angle [0-360), ViewType_Progress -
                 // [0, 100]
  Vertex2i verts[3]; // ViewType_Orientation - 3 triangle verts
  bool frame;        // Draw single pixel frame
  bool dirty;        // State is dirty
};
DrawState drawState = {};

void drawCenterString(const char *buf, int x, int y, int size = 1,
                      int color = COLOR_WHITE) {
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(size);
  display.setTextColor(color);
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(x - w / 2, y - h / 2);
  display.print(buf);
}

void progressViewClear() {
  display.drawRect(4, 32, 4 + drawState.value, 8, COLOR_BLACK);
  drawState.dirty = true;
}

void progressViewDraw(int16_t value) {
  display.drawRect(4, 32, 4 + value, 8, COLOR_WHITE);
  drawState.value = value;
  drawState.dirty = true;
}

void orientationViewClear() {
  display.drawTriangle(drawState.verts[0].x, drawState.verts[0].y,
                       drawState.verts[1].x, drawState.verts[1].y,
                       drawState.verts[2].x, drawState.verts[2].y, COLOR_BLACK);
  drawState.dirty = true;
}

void orientationViewDraw(int16_t angle) {
  float s = sin(radians(angle));
  float c = cos(radians(angle));
  Vertex2i shape[3] = {{-5, -7}, {0, 10}, {5, -7}};
  Vertex2i rotated[3];
  for (int i = 0; i < 3; ++i) {
    rotated[i].x = (SCREEN_WIDTH / 2) + (shape[i].x * c) - (shape[i].y * s);
    rotated[i].y =
        ((SCREEN_HEIGHT / 2) + 12) + (shape[i].y * c) + (shape[i].x * s);
  }
  display.drawTriangle(rotated[0].x, rotated[0].y, rotated[1].x, rotated[1].y,
                       rotated[2].x, rotated[2].y, COLOR_WHITE);

  // Store state
  drawState.value = angle;
  memcpy(drawState.verts, rotated, sizeof(Vertex2i) * 3);
  drawState.dirty = true;
}

void counterViewClear() {
  String valueString(drawState.value);
  drawCenterString(valueString.c_str(), SCREEN_WIDTH / 2,
                   SCREEN_HEIGHT / 2 + 12, 3, COLOR_BLACK);
  drawState.dirty = true;
}

void counterViewDraw(int16_t value) {
  String valueString(value);
  drawCenterString(valueString.c_str(), SCREEN_WIDTH / 2,
                   SCREEN_HEIGHT / 2 + 12, 3, COLOR_WHITE);
  drawState.value = value;
  drawState.dirty = true;
}

void drawFrame(bool frame) {
  if (drawState.frame == frame) {
    return;
  }
  display.drawRect(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1,
                   frame ? COLOR_WHITE : COLOR_BLACK);
  drawState.frame = frame;
  drawState.dirty = true;
}

void drawViewHeader(uint8_t view) {
  if (drawState.view == view) {
    return;
  }

  if (view == ViewType_None) {
    display.fillRect(1, 1, SCREEN_WIDTH - 3, 12, COLOR_BLACK);
  } else {
    display.fillRect(1, 1, SCREEN_WIDTH - 3, 12, COLOR_WHITE);
    char *buf = "";
    switch (view) {
    case ViewType_Progress:
      buf = "PROGRESS";
      break;
    case ViewType_Orientation:
      buf = "ORIENTATION";
      break;
    case ViewType_Counter:
      buf = "COUNTER";
      break;
    }
    drawCenterString(buf, 64, 8, 1, COLOR_BLACK);
  }
  drawState.dirty = true;
}

void handleDriverStationMessage(uint16_t message) {
  // Update lights based on the message from Driver Station
  uint16_t messageType = GET_MESSAGE_TYPE(message);

  // [0..8] LED ON/OFF state
  if (messageType == MessageType_LedsUpdate) {
    digitalWrite(BUTTON_LED_1, bitRead(message, 0) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_2, bitRead(message, 1) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_3, bitRead(message, 2) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_4, bitRead(message, 3) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_5, bitRead(message, 4) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_6, bitRead(message, 5) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_7, bitRead(message, 6) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_8, bitRead(message, 7) ? HIGH : LOW);
    digitalWrite(BUTTON_LED_9, bitRead(message, 8) ? HIGH : LOW);
  }

  if (messageType == MessageType_ViewUpdate) {
    uint16_t viewType = GET_VIEW_TYPE(message);
    uint16_t frame = GET_FRAME(message);
    uint16_t value = GET_VALUE(message);

    drawViewHeader(viewType);
    drawFrame(frame);
    // Clear previous state
    if (viewType != drawState.view || value != drawState.value) {
      switch (drawState.view) {
      case ViewType_Progress:
        progressViewClear();
      case ViewType_Orientation:
        orientationViewClear();
        break;
      case ViewType_Counter:
        counterViewClear();
        break;
      }

      // Draw current state
      switch (viewType) {
      case ViewType_Progress:
        progressViewDraw(value);
        break;
      case ViewType_Orientation:
        orientationViewDraw(value);
        break;
      case ViewType_Counter:
        counterViewDraw(value);
        break;
      }
    }
    drawState.view = viewType;
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
  joystick.setXAxis(enc1.read() / 4);
  joystick.setYAxis(enc2.read() / 4);

  // Read the joystick output from the Driver Station
  int32_t dsMessage = readJoystickOutput();
  if (dsMessage >= 0) {
    handleDriverStationMessage(dsMessage);
  }

  // ///////////////////////////////////////////////////
  // // Build test message
  // static unsigned long timeStart = micros();
  // unsigned long timePeriod = (micros() - timeStart) % 3000000;
  // uint16_t message = SET_MESSAGE_TYPE(MessageType_ViewUpdate);
  // if (timePeriod < 1000000) {
  //   static uint16_t angle = 0;
  //   message |= SET_VIEW_TYPE(ViewType_Orientation);
  //   message |= SET_FRAME(1);
  //   message |= SET_VALUE(angle);
  //   angle = (angle + 1) % 360;
  // } else if (timePeriod < 2000000) {
  //   static uint16_t value = 0;
  //   message |= SET_VIEW_TYPE(ViewType_Progress);
  //   message |= SET_FRAME(0);
  //   message |= SET_VALUE(value);
  //   value = (value + 1) % 100;
  // } else {
  //   static uint16_t count = 0;
  //   message |= SET_VIEW_TYPE(ViewType_Counter);
  //   message |= SET_FRAME(count > 500);
  //   message |= SET_VALUE(count / 10);
  //   count = (count + 1) % 1000;
  // }
  // handleDriverStationMessage(message);
  // ///////////////////////////////////////////////////

  static bool onceDirty = false;

  // Blit display if dirty
  if (drawState.dirty) {
    // Clear screen from startup images.
    if (!onceDirty) {
      display.fillScreen(COLOR_BLACK);
      onceDirty = true;
    }
    display.display();
    drawState.dirty = false;
  }

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
