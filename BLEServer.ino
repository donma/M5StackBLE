/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5Stack.h>
#define SERVICE_UUID        "0000fff0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
String Message = "DMESSAGE";
BLEService *pService;
BLEAdvertising *pAdvertising;
BLECharacteristic *pCharacteristic;

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.println("Starting BLE work!");

  BLEDevice::init("D51-MASTER");
  BLEServer *pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setValue(Message.c_str());
  pService->start();
  pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  M5.Lcd.println("Success Broadcasting..");
}

void loop() {

  if (M5.BtnA.wasPressed()) {
    Message = "<<";
   pCharacteristic->setValue(Message.c_str());
    M5.Lcd.print("Set Value to ");
    M5.Lcd.println(Message.c_str());
  }
  else if (M5.BtnC.wasPressed()) {
    Message = ">>";
   pCharacteristic->setValue(Message.c_str());
    M5.Lcd.print("Set Value to ");
    M5.Lcd.println(Message.c_str());
  }
  else if (M5.BtnB.wasPressed()) {
    Message = "D51c";
    pCharacteristic->setValue(Message.c_str());
    M5.Lcd.print("Set Value to ");
    M5.Lcd.println(Message.c_str());
  }
  M5.update();
  delay(500);
}
