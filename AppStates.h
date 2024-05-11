// This file declares and implements the State* classes, which are the child of State class
// and contains the logic of each state of our application.
//
// Pierre Rossel   2024-04-26  Initial version

#pragma once

#include "StateMachine.h"

void checkSerial();

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

class StateTestAccelerometer : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};

class StateTestCamera : public State
{
  void enter();

  // Loop is declared here, but implemented below, after the declaration of other states
  State *loop();

  void exit();
};

