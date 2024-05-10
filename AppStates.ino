// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

#include "AppStates.h"

// ************************************************************
//                           STATE INIT
// ************************************************************

void StateInit::enter() {
  Serial.println(">>>>>>>>>>>>> StateInit.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

}

// loop
State *StateInit::loop() {
  // wait for 5 seconds
  if (getStateTime() > 5000) {
    return new StateIdle();
  }

  return NULL;
}

// exit
void StateInit::exit() {
  Serial.println("<<<<<<<<<<<<< StateInit.exit()");
  digitalWrite(LED_BUILTIN, LOW);
}


// ************************************************************
//                         STATE PICTURE
// ************************************************************

void StatePicture::enter() {
  Serial.println(">>>>>>>>>>>>> StatePicture.enter()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Turn off accelrometer
  disableAccelerometer();

  // Turn on camera
  setupCamera();

  // TODO: Turn on all leds


  // take a picture
  capture();
}

// loop
State *StatePicture::loop() {

  return new StateIdle();
}

// exit
void StatePicture::exit() {
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

}

State *StateIdle::loop()
{
  // Every second, turn the next LED on or off on the strip
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000)
  {
    lastUpdate = millis();
    static int led = 0;
    strip.clear();
    strip.setPixelColor(led, strip.Color(255, 255, 255));
    strip.show();
    led++;
    if (led >= strip.numPixels())
    {
      led = 0;
    }
  }


  return NULL;
}

void StateIdle::exit()
{
  Serial.println("<<<<<<<<<<<<< StateIdle.exit()");
  strip.clear();
  strip.show();
}

