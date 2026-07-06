

#include <Arduino.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>

#if __has_include(<U8g2lib.h>)
  #include <Wire.h>
  #include <U8g2lib.h>
  #define HAS_OLED 1
#else
  #define HAS_OLED 0
#endif

const uint16_t kRecvPin = 14;
const uint16_t kCaptureBufferSize = 1024;
const uint8_t  kTimeout = 50;      
const uint32_t kBaudRate = 115200;

IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;

#if HAS_OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif

uint16_t captureCount = 0;

void updateOLED() {
#if HAS_OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 12, "IR Code Capture");
  u8g2.drawStr(0, 28, "Point remote at");
  u8g2.drawStr(0, 40, "receiver, press a btn");
  u8g2.setCursor(0, 58);
  u8g2.print("Captured: ");
  u8g2.print(captureCount);
  u8g2.sendBuffer();
#endif
}

void setup() {
  Serial.begin(kBaudRate);
  delay(500);

#if HAS_OLED
  u8g2.begin();
#endif

  irrecv.setUnknownThreshold(12);
  irrecv.enableIRIn();
  updateOLED();

  Serial.println();
  Serial.println("======================================================");
  Serial.println(" IR CODE CAPTURE - ready.");
  Serial.println(" Point your AC remote at the IR receiver and press a button.");
  Serial.println("======================================================");
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println();
    Serial.println("---------------------- CAPTURED ----------------------");
    Serial.println(resultToHumanReadableBasic(&results));
    Serial.println(resultToSourceCode(&results));
    Serial.println("--------------------------------------------------------");
    Serial.println(">> Copy the array above into ac_codes.h and rename it.");

    captureCount++;
    updateOLED();
    irrecv.resume();
  }
}
