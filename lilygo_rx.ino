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

int lastPacketNum = 0;
int totalReceived = 0;
int totalLost = 0;
int totalCorrupt = 0;

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

  Serial.println("Ready to receive...");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String raw = "";
    while (LoRa.available()) {
      raw += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    int sep = raw.lastIndexOf('|');
    String payload = raw.substring(0, sep);
    String crcRecv = raw.substring(sep + 1);

    uint32_t crcCalc = esp_crc32_le(0, (const uint8_t*)payload.c_str(), payload.length());
    String crcCalcStr = String(crcCalc, HEX);

    bool valid = crcCalcStr.equalsIgnoreCase(crcRecv);

    if (valid) {
      int idx = payload.indexOf('#');
      int pktNum = (idx != -1) ? payload.substring(idx + 1).toInt() : 0;
      if (lastPacketNum != 0 && pktNum > lastPacketNum + 1) {
        totalLost += (pktNum - lastPacketNum - 1);
      }
      lastPacketNum = pktNum;
      totalReceived++;
    } else {
      totalCorrupt++;
    }

    Serial.print("[RX] "); Serial.print(payload);
    Serial.print(" | Valid: "); Serial.print(valid ? "YES" : "NO (CORRUPT)");
    Serial.print(" | RSSI: "); Serial.print(rssi);
    Serial.print(" dBm | SNR: "); Serial.print(snr);
    Serial.print(" | Lost: "); Serial.print(totalLost);
    Serial.print(" | Corrupt: "); Serial.println(totalCorrupt);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(valid ? "<< RX VALID <<" : "<< RX CORRUPT <<");
    display.println(payload);
    display.print("RSSI:"); display.println(rssi);
    display.print("Lost:"); display.print(totalLost);
    display.print(" Cor:"); display.println(totalCorrupt);
    display.display();
  }
}