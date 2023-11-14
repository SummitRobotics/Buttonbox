#define INPUT_PIN A5

void setup() {
    Serial.begin(9600);
    pinMode(INPUT_PIN, INPUT);
}

void loop() {
    int result = readAnalogButton();
    Serial.println(result);
}

int near(int val, int ref, int dist = 2) {
  return abs(val - ref) <= dist;
}

int readAnalogButton() {
  int val = analogRead(INPUT_PIN);
  if (near(val, 772)) return 1;
  if (near(val, 855)) return 2;
  if (near(val, 911)) return 3;
  if (near(val, 931)) return 4;
  if (near(val, 949)) return 5;
  if (near(val, 959)) return 6;
  if (near(val, 970)) return 7;
  return 0;
}