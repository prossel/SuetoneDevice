/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

*/

/* This example domenstrates the Bluetooth data transparent transmission function. Burn the code, open serial monitor, turn on the BLE debugger on the phone, then,
 * 1. you can see the data sent by ESP32-S3--see APP usage image
 * 2. send data to ESP32-S3 by the input box of BLE debugger--see APP usage image
 * This example originates from BLE_uart sample
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
  { // Transmit data when the Bluetooth is connected.
    pTxCharacteristic->setValue("Hello this is a long string"); // Send char string
    pTxCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent

    pTxCharacteristic->setValue("DFRobot"); // Send char string
    pTxCharacteristic->notify();
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
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