#include <Arduino.h>
#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "ac_codes.h"
#include <SinricPro.h>
#include <SinricProWindowAC.h> 
#include <U8g2lib.h>
#include <Wire.h>

// WiFi and SinricPro Credentials
#define WIFI_SSID         "Your wifi name"
#define WIFI_PASS         "Your wifi password"
#define APP_KEY           "APP key from sinric pro"
#define APP_SECRET        "APP secret from sinric pro"
#define AC_DEVICE_ID      "DEVICE ID from sinric pro" 


const uint16_t kIrLed = 4;
#define LED_PIN 2 

IRsend irsend(kIrLed);
const uint16_t kIrFrequency = 38;

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
String lastCommand = "Ready";

void blinkLED() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void showStatus() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 10, "Smart AC (Window)");
  u8g2.setCursor(0, 30);
  u8g2.print("Status: "); u8g2.print(lastCommand);
  u8g2.sendBuffer();
}


bool onPowerState(const String &deviceId, bool &state) {
  if (state) {
    irsend.sendRaw(POWER_ON, sizeof(POWER_ON)/sizeof(POWER_ON[0]), kIrFrequency);
    lastCommand = "Power ON";
  } else {
    irsend.sendRaw(POWER_OFF, sizeof(POWER_OFF)/sizeof(POWER_OFF[0]), kIrFrequency);
    lastCommand = "Power OFF";
  }
  showStatus();
  blinkLED();
  return true;
}


bool onTargetTemperature(const String &deviceId, float &temperature) {
  if (temperature > 24) { 
    irsend.sendRaw(TEMP_UP, sizeof(TEMP_UP)/sizeof(TEMP_UP[0]), kIrFrequency);
    lastCommand = "Temp UP";
  } else {
    irsend.sendRaw(TEMP_DOWN, sizeof(TEMP_DOWN)/sizeof(TEMP_DOWN[0]), kIrFrequency);
    lastCommand = "Temp DOWN";
  }
  showStatus();
  blinkLED();
  return true;
}


bool onThermostatMode(const String &deviceId, String &mode) {
  if (mode == "COOL") {
    irsend.sendRaw(MODE_COOL, sizeof(MODE_COOL)/sizeof(MODE_COOL[0]), kIrFrequency);
    lastCommand = "Mode: COOL";
  } else if (mode == "AUTO") {
    irsend.sendRaw(MODE_AUTO, sizeof(MODE_AUTO)/sizeof(MODE_AUTO[0]), kIrFrequency);
    lastCommand = "Mode: AUTO";
  }
  showStatus();
  blinkLED();
  return true;
}

void setupSinricPro() {
  SinricProWindowAC &myAC = SinricPro[AC_DEVICE_ID];
  myAC.onPowerState(onPowerState);
  myAC.onTargetTemperature(onTargetTemperature);
  myAC.onThermostatMode(onThermostatMode);

  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  irsend.begin();
  u8g2.begin();
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  setupSinricPro();
  showStatus();
}

void loop() {
  SinricPro.handle();
}