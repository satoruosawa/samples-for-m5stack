#include <BLE2902.h>
#include <BLEDevice.h>
#include <M5Stack.h>

#define SERVICE_UUID "00002220-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "00002221-0000-1000-8000-00805F9B34FB"

BLECharacteristic* pCharacteristic = NULL;
BLEServer* pServer = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // The program stops if you call M5.lcd.print function in the callback.
    Serial.println("connect");
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    // The program stops if you call M5.lcd.print function in the callback.
    Serial.println("disconnect");
    deviceConnected = false;
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onNotify(BLECharacteristic* pCharacteristic) {
    // The program stops if you call M5.lcd.print function in the callback.
    Serial.println("onNotify");
  }
};

void setup() {
  M5.begin();
  BLEDevice::init("M5Stack");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  pCharacteristic->setValue((uint8_t*)&value, 4);
  pService->start();
  pServer->startAdvertising();
  Serial.println("Start advertising...  ");
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("BLE Notify");
  M5.Lcd.setCursor(0, 18);
  M5.Lcd.println("Start advertising...  ");
}

void refreshRate() {
  static int frame_count = 0;
  static float prev_time = 0.0;
  static float rate = 60.0;
  frame_count++;
  float time = millis() / 1000.0 - prev_time;
  if (time >= 0.5f) {
    rate = frame_count / time;
    rate = frame_count / time;
    frame_count = 0;
    prev_time = millis() / 1000.0;
  }
  M5.Lcd.setCursor(0, 54);
  M5.Lcd.printf("Refresh Rate %5.2f", rate);
}

void loop() {
  refreshRate();
  if (deviceConnected) {
    M5.Lcd.setCursor(0, 18);
    M5.Lcd.println("Connected             ");
    static unsigned long prevUpdate = 0;
    if (millis() > prevUpdate + 1000) {
      prevUpdate = millis();
      pCharacteristic->setValue((uint8_t*)&value, 4);
      pCharacteristic->notify();
      M5.Lcd.setCursor(0, 36);
      M5.Lcd.printf("Count%3d", value);
      value++;
      delay(10);
    }
  }
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    Serial.println("Restart advertising");
    M5.Lcd.setCursor(0, 18);
    M5.Lcd.println("Restart advertising...");
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}