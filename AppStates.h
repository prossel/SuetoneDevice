// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

#pragma once

#include <Adafruit_NeoPixel.h>

const int stripPin = D2;

Adafruit_NeoPixel strip(12, stripPin, NEO_GRB + NEO_KHZ800);
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




#include "StateMachine.h"

class StateInit : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};

class StatePicture : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};

class StateIdle : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};

class StatePulse : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};
