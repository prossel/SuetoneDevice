
/*
    Suetone code for Firebeetle 2 ESP32-S3
    
    BLE Inspired from https://wiki.dfrobot.com/SKU_DFR0975_FireBeetle_2_Board_ESP32_S3_Advanced_Tutorial#target_2

    Pierre Rossel   2024-05-05   Initial version
*/

bool deviceConnected = false;

void setup()
{
  Serial.begin(115200);
  BLEBegin(); // Init Bluetooth
  
  setupCamera();
}

void loop()
{
  if (deviceConnected)
  {
    static unsigned long nextSend = 0;
    unsigned long ms = millis();

    if (ms >= nextSend)
    {
      //sendTestFile(14 * 1024);

      capture();

      nextSend = millis() + 5000;
    }
  }
}

