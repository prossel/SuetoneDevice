
/*
    Suetone code for Firebeetle 2 ESP32-S3

    BLE Inspired from https://wiki.dfrobot.com/SKU_DFR0975_FireBeetle_2_Board_ESP32_S3_Advanced_Tutorial#target_2

    Pierre Rossel   2024-05-05   Initial version
*/

bool deviceConnected = false;

// #include <Adafruit_NeoPixel.h>

// const int stripPin = D2;

// Adafruit_NeoPixel strip(12, stripPin, NEO_GRB + NEO_KHZ800);
// // Argument 1 = Number of pixels in NeoPixel strip
// // Argument 2 = Arduino pin number (most are valid)
// // Argument 3 = Pixel type flags, add together as needed:
// //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// // NEOPIXEL BEST PRACTICES for most reliable operation:
// // - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// // - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// // - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// // - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
// //   connect GROUND (-) first, then +, then data.
// // - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
// //   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// // (Skipping these may work OK on your workbench but can fail in the field)



#include "AppStates.h"

StateMachine sm;

// delcaration of functions implemented in other .ino files to avoid Intellitype errors
void setupCamera();
void disableCamera();
void capture();
void sendTestFile(int size);
void setupAccelerometer();
void disableAccelerometer();
void dumpAccelGyro();
void BLEBegin();


void setup()
{
  Serial.begin(115200);
  BLEBegin();  // Init Bluetooth

  sm.changeState(new StateInit());
}

void loop() {
  // static bool cameraInit = false;
  // if (!cameraInit && millis() > 10000) {
  //   cameraInit = true;
  //   setupCamera();
  //   disableCamera();
  // }

  static char state = ' ';

  // Change state from serial
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'c' || c == 'a' || c == 'o') {
      if (c != state) {
        Serial.print("Switching to ");
        Serial.println(c);

        // Disable previous state
        switch (state) {
          case 'c':
            disableCamera();
            break;
          case 'a':
            disableAccelerometer();
            break;
        }

        // Enable new state
        switch (c) {
          case 'c':
            setupCamera();
            break;

          case 'a':
            setupAccelerometer();
            break;
        }
      }

      state = c;
    }
  }


  // Run current state
  switch (state) {
    case 'c':
      testCamera();
      break;
    case 'a':
      testAccelerometer();
      break;
  }

  delay(10);
}

void testCamera() {
  static unsigned long nextSend = 0;
  unsigned long ms = millis();

  if (ms >= nextSend) {
    // sendTestFile(14 * 1024);

    if (deviceConnected) {
      capture();
    }

    nextSend = millis() + 5000;
  }
}

void testAccelerometer() {

  static unsigned long nextSend = 0;
  unsigned long ms = millis();

  if (ms >= nextSend) {
    dumpAccelGyro();
    nextSend = millis() + 500;
  }
}