// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

#include "Accelerometer.h"
#include "BLE.h"
#include "Camera.h"

// Pin to read battery voltage. There is a votage divisor by 1.33 so 4.2V will be 3.15V
#define BATTERY_PIN A3
float batteryVoltage = 0;


#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 24
#define STRIP_PIN D2

Adafruit_NeoPixel strip(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include "AppStates.h"

// Shared motion detector
MotionDetector motion;

//  orientation when object is layed down
int16_t avFlat[3] = {0, 0, 0};

extern StateMachine sm;

void checkSerial()
{
  // Change state from serial
  while (Serial.available() > 0)
  {
    char c = Serial.read();

    switch (c)
    {
    case 'c':
      sm.changeState(new StateTestCamera());
      break;
    case 'a':
      sm.changeState(new StateTestAccelerometer());
      break;
    case 'o':
      sm.changeState(NULL);
      break;
    case 'd':
      MotionDetector::debug = !MotionDetector::debug;
    }
  }
}

// Read battery voltage on A3
void readBatteryVoltage()
{
  pinMode(BATTERY_PIN, INPUT);
  uint16_t val = analogRead(BATTERY_PIN);
  // Serial.print("Battery raw value: ");
  // Serial.println(val);

  batteryVoltage = val*3.3 / 4095 * 1.33;

  // // Every 10 sec send on serial 
  // static unsigned long lastSend = -1;
  // unsigned long ms = millis();
  // if (ms - lastSend > 10000)
  // {
  //   lastSend = ms;
  //   Serial.print("Battery voltage: ");
  //   Serial.println(batteryVoltage);
  // } 

}

// Led strip breath effect
void breathStrip(float period, unsigned long start, int red = 255, int green = 255, int blue = 255)
{
  static unsigned long lastUpdate = 0;
  static float brightness = 0;

  unsigned long ms = millis();
  if (ms - lastUpdate > period / 100)
  {
    lastUpdate = ms;
    brightness = 128 + 127 * cos(2 * PI * (ms - start) / period - PI);

    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255));
    }
    strip.show();
  }
}

// ************************************************************
//                           STATE INIT
// ************************************************************

void StateInit::enter()
{
  Serial.println(">>>>>>>>>>>>> StateInit.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  BLEBegin(); // Init Bluetooth

  // Turn on accelrometer by default.
  // Anytime we are in a state that requires the camera, we should turn off the accelerometer,
  // enable the camera, and then disable the camera and re-enable the accelerometer when we leave that state.
  setupAccelerometer();

  motion.start(200);
}

State *StateInit::loop()
{
  // Breath effect
  breathStrip(6000, stateEnterMs, 0, 255, 0);

  unsigned long ms = millis();

  // reset timer when motion is detected. If no motion is detected for 5 seconds, save the orientation and to to picture state
  motion.update();
  if ((motion.motionDetected() ? ms - motion.getLastMotionMs() : getStateTime()) > 5000)
  {
    Serial.println("No move detected");

    // init flat vector
    accelgyro.getAcceleration(&avFlat[0], &avFlat[1], &avFlat[2]);

    return new StatePicture();
  }

  return NULL;
}

// exit
void StateInit::exit()
{
  Serial.println("<<<<<<<<<<<<< StateInit.exit()");
  digitalWrite(LED_BUILTIN, LOW);
}

// ************************************************************
//                         STATE PICTURE
// ************************************************************

void StatePicture::enter()
{
  Serial.println(">>>>>>>>>>>>> StatePicture.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Turn off accelrometer
  disableAccelerometer();

  // Turn on camera
  setupCamera();
}

// loop
State *StatePicture::loop()
{
  // Turn ON all leds white full power
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(60, 60, 60)); // RGB value for white
  }
  strip.show();

  delay(200);

  // take a picture
  capture();

  // turn off leds
  strip.clear();
  strip.show();

  return new StateIdle();
}

// exit
void StatePicture::exit()
{
  Serial.println("<<<<<<<<<<<<< StatePicture.exit()");
  digitalWrite(LED_BUILTIN, LOW);

  // Turn off camera
  disableCamera();

  // Turn on accelrometer
  setupAccelerometer();
}

// ************************************************************
//                           STATE IDLE
// ************************************************************

void StateIdle::enter()
{
  Serial.println(">>>>>>>>>>>>> StateIdle.enter()");
  pinMode(LED_BUILTIN, OUTPUT);

  strip.clear();
  strip.show();

  motion.start(1000);
}

State *StateIdle::loop()
{
  // If motion is detected go to pulse state
  // wait 1s before changing state
  motion.update();
  if (motion.motionDetected() && getStateTime() > 1000)
  {
    return new StatePulse();
  }

  //readBatteryVoltage();

  return NULL;
}

void StateIdle::exit()
{
  Serial.println("<<<<<<<<<<<<< StateIdle.exit()");
}

// ************************************************************
//                           STATE PULSE
// ************************************************************

void StatePulse::enter()
{
  Serial.println(">>>>>>>>>>>>> StatePulse.enter()");

  strip.clear();
  strip.show();

  motion.start(200);
}

State *StatePulse::loop()
{
  // Breath effect
  breathStrip(6000, stateEnterMs);

  // if still during 1 s go to picture state
  motion.update();
  if (millis() - motion.getLastMotionMs() > 1000)
  {
    return new StatePicture();
  }

  return NULL;
}

void StatePulse::exit()
{
  Serial.println("<<<<<<<<<<<<< StatePulse.exit()");
  digitalWrite(LED_BUILTIN, LOW);
  strip.clear();
  strip.show();
}

// ************************************************************
//                           STATE TEST ACCELEROMETER
// ************************************************************

void StateTestAccelerometer::enter()
{
  Serial.println(">>>>>>>>>>>>> StateTestAccelerometer.enter()");

  disableCamera();
  setupAccelerometer();
}

State *StateTestAccelerometer::loop()
{
  static unsigned long nextSend = 0;
  unsigned long ms = millis();

  if (ms >= nextSend)
  {
    dumpAccelGyro();
    nextSend = millis() + 500;
  }

  return NULL;
}

void StateTestAccelerometer::exit()
{
  Serial.println("<<<<<<<<<<<<< StateTestAccelerometer.exit()");

  disableAccelerometer();
  enableCamera();
}

// ************************************************************
//                           STATE TEST CAMERA
// ************************************************************

void StateTestCamera::enter()
{
  Serial.println(">>>>>>>>>>>>> StateTestCamera.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  disableAccelerometer();
  setupCamera();
}

State *StateTestCamera::loop()
{
  static unsigned long nextSend = 0;
  unsigned long ms = millis();

  if (ms >= nextSend)
  {
    // sendTestFile(14 * 1024);

    if (deviceConnected)
    {
      capture();
    }

    nextSend = millis() + 5000;
  }

  return NULL;
}

void StateTestCamera::exit()
{
  Serial.println("<<<<<<<<<<<<< StateTestCamera.exit()");

  disableCamera();
  setupAccelerometer();
}
