#include <Arduino.h>
#include <ArduinoOSC.h>
#include <M5Stack.h>
#include <WiFi.h>
#include "./wifi-info.h"

OscWiFi osc;
const String SSID = WIFI_SSID;          // "xxxx"
const String PASSWORD = WIFI_PASSWORD;  // "xxxx"
const int PORT = 12000;
const char* TARGEET_IP = "10.0.10.135";
const int TARGET_PORT = 10000;

void setup() {
  M5.begin();
  WiFi.begin(SSID.c_str(), PASSWORD.c_str());
  M5.Lcd.printf("Connecting to the WiFi AP: %s ", SSID.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println(" connected.");
  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  osc.begin(PORT);
}

int count = 0;

void loop() {
  count++;
  float f = 34.56F;
  double d = 78.987;
  String s = "hello";
  bool b = true;
  osc.parse();  // should be called
  osc.send(TARGEET_IP, TARGET_PORT, "/test", count, f, d, s, b);
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.print("Send OSC to [");
  M5.Lcd.print(TARGEET_IP);
  M5.Lcd.print(":");
  M5.Lcd.print(TARGET_PORT);
  M5.Lcd.print("] ");
  M5.Lcd.print(count);
  M5.Lcd.println(" times.");
  delay(1000);
}
