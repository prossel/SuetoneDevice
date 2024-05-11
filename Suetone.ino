
/*
    Suetone code for Firebeetle 2 ESP32-S3

    BLE Inspired from https://wiki.dfrobot.com/SKU_DFR0975_FireBeetle_2_Board_ESP32_S3_Advanced_Tutorial#target_2

    Pierre Rossel   2024-05-05   Initial version
*/

#include "AppStates.h"

StateMachine sm;

void setup()
{
  Serial.begin(115200);

  sm.changeState(new StateInit());
}

void loop()
{
  checkSerial();

  sm.loop();

  delay(10);
}
