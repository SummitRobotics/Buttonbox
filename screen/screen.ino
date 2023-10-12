/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
 #include <Encoder.h>

const int encoder_pin1 = 3;
const int encoder_pin2 = 4;

// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  8


// create an instance of the library
TFT screen = TFT(cs, dc, rst);

Encoder myEnc(encoder_pin1, encoder_pin2);

void setup() {

  //initialize the library
  screen.begin();

  // clear the screen with a black background
  screen.background(0, 0, 255);
  //set the text size
  screen.setTextSize(2);

  Serial.begin(9600);

    // set a random font color
  screen.stroke(255, 255, 255);

}

  int lastValue = -1;
  int lastWidth = 0;
void loop() {

  int value = myEnc.read();
  if(lastValue != value) {
    value = value < -320? -320 : value;
    value = value > 320? 320 : value;
    myEnc.write(value);
    int width = value/4;
    if (lastWidth != width) {
    char text[80];
    sprintf(text, "%d", value);
    Serial.println(text);
    screen.stroke(0, 0, 0);
    screen.fill(0, 0, 0);
    screen.rect(6, 57, 48, 14);
    screen.stroke(255, 255, 255);

    // print Hello, World! in the middle of the screen
    screen.text(text, 6, 57);

    screen.fill(0, 0, 0);
    screen.stroke(0, 0, 0);
    screen.rect(0, 0, 160, 10);
    screen.fill(255, 255, 255);
    screen.stroke(255, 255, 255);
    if (width < 0) {
    screen.rect(80 + width, 0, abs(width), 10);
    }
    else {
    screen.rect(80, 0, width, 10);
    }
    }
    lastWidth = width;
  }

  lastValue = value;


  
  // wait 200 miliseconds until change to next color
  // delay(200);
}
