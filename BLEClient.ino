/**
 * A BLE client example that is rich in capabilities.
 */

#include "BLEDevice.h"
#include <M5Stack.h>
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000fff0-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID  charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");


static BLEAddress *pServerAddress;
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    M5.Lcd.print("Notify callback for characteristic ");
    M5.Lcd.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    M5.Lcd.print(" of data length ");
    M5.Lcd.println(length);
}

bool connectToServer(BLEAddress pAddress) {
    M5.Lcd.print("Forming a connection to ");
    M5.Lcd.println(pAddress.toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    M5.Lcd.println(" - Created client");

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    M5.Lcd.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      M5.Lcd.print("Failed to find our service UUID: ");
      M5.Lcd.println(serviceUUID.toString().c_str());
      return false;
    }
    M5.Lcd.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      M5.Lcd.print("Failed to find our characteristic UUID: ");
      M5.Lcd.println(charUUID.toString().c_str());
      return false;
    }
    M5.Lcd.println(" - Found our characteristic");

    // Read the value of the characteristic.
    std::string value = pRemoteCharacteristic->readValue();
    M5.Lcd.print("The characteristic value was: ");
    M5.Lcd.println(value.c_str());

    pRemoteCharacteristic->registerForNotify(notifyCallback);
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    M5.Lcd.print("BLE Advertised Device found: ");
    M5.Lcd.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {

      // 
      M5.Lcd.print("Found our device!  address: "); 
      advertisedDevice.getScan()->stop();

      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  M5.begin();
  M5.Lcd.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      M5.Lcd.println("We are now connected to the BLE Server.");
      connected = true;
    } else {
      M5.Lcd.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
     std::string value = pRemoteCharacteristic->readValue();
     M5.Lcd.println(value.c_str());
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop
