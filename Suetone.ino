/*
    Suetone code for Firebeetle 2 ESP32-S3
    
    BLE Inspired from https://wiki.dfrobot.com/SKU_DFR0975_FireBeetle_2_Board_ESP32_S3_Advanced_Tutorial#target_2

    Pierre Rossel   2024-05-05   Initial version
*/

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Bluetooth connect/disconnect. Auto triggered when connection/disconnection event occurs.
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  { // Execute this function when Bluetooth is connected.
    Serial.println("Bluetooth connected");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  { // Execute this function when Bluetooth is disconnected
    Serial.println("Bluetooth disconnected");
    deviceConnected = false;
    delay(500);                  // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
  }
};

/****************Data Receiving*************/
/****************************************/
// Process received Bluetooth data. Auto triggered when data received.
class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue(); // Receive data, and assign it to rxValue

    // if(rxValue == "ON"){Serial.println("Turn light on");}   //Determine whether the received character is "ON"

    if (rxValue.length() > 0)
    {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++)
      {
        Serial.print(rxValue[i]);
      }
      Serial.println();
      Serial.println("*********");
    }
  }
};
/***************************************/
/****************************************/

void setup()
{
  Serial.begin(115200);
  BLEBegin(); // Init Bluetooth
}

void loop()
{
  /****************Data Transmitting*************/
  /****************************************/
  if (deviceConnected)
  {
    sendTestFile(14 * 1024);
  }
  /****************************************/
  /****************************************/
}

void BLEBegin()
{
  // Create the BLE Device
  BLEDevice::init(/*BLE Name*/ "Suetone");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();

  Serial.println("Waiting a client connection to notify...");
}

void sendToBLE(uint8_t *data, int length)
{
  pTxCharacteristic->setValue(data, length);
  pTxCharacteristic->notify();
  delay(10); // bluetooth stack will go into congestion, if too many packets are sent
}

void sendToBLE(const String &data)
{
  sendToBLE((uint8_t *)data.c_str(), data.length());
}

void sendTestFile(int dataLength)
{
  static unsigned long nextSend = 0;
  unsigned long ms = millis();

  if (ms >= nextSend)
  {

    // Use the maximum value given by the stack as an error when using to big of a chunk size
    int chunkSize = 500;

    // Simulate data
    uint8_t data[chunkSize];
    for (int i = 0; i < chunkSize; i++)
    {
      data[i] = i % 10 + '0';
    }
    data[chunkSize - 1] = '\n';

    // Send file name and size (ex: FILE image.jpg SIZE {dataLength} CHUNK {chunkSize}\n))
    String message = "{\"file\": \"test.txt\", \"size\": " + String(dataLength) + ", \"chunk\": " + String(chunkSize) + "}";
    Serial.println(message);
    sendToBLE(message);

    // Transmit data when the Bluetooth is connected.
    for (int i = 0; i < dataLength; i += chunkSize)
    {
      sendToBLE(&data[0], i < (dataLength - chunkSize) ? chunkSize : dataLength - i);
    }

    // Send end of transmission signal
    sendToBLE("END_OF_TRANSMISSION");

    Serial.println("Sent " + String(dataLength) + " bytes in " + String(millis() - ms) + " ms");

    nextSend = millis() + 5000;
  }
}