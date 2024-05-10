
/*
    Suetone code for Firebeetle 2 ESP32-S3

    BLE Inspired from https://wiki.dfrobot.com/SKU_DFR0975_FireBeetle_2_Board_ESP32_S3_Advanced_Tutorial#target_2

    Pierre Rossel   2024-05-05   Initial version
*/

bool deviceConnected = false;

void setup() {
  Serial.begin(115200);
  BLEBegin();  // Init Bluetooth

  // setupCamera();
  // setupAccelerometer();
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