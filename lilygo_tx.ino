#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_crc.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI0 26
#define BAND 915E6

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SSD1306 display(128, 64, &Wire, -1);

int packetCounter = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 failed"));
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa failed!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LoRa Error!");
    display.display();
    while (1);
  }

  Serial.println("Ready to send test...");
}

void loop() {
  packetCounter++;
  String payload = "HELLO#" + String(packetCounter);
  uint32_t crc = esp_crc32_le(0, (const uint8_t*)payload.c_str(), payload.length());

  String msg = payload + "|" + String(crc, HEX);

  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();
  Serial.println("[TX] Sent " + msg);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(">> TX SENDING >>");
  display.print("Pkt: "); display.println(packetCounter);
  display.print("CRC: "); display.println(crc, HEX);
  display.display();

  delay(2000);
}