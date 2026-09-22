# WANADRI — LilyGO T3 LoRa TX/RX Test

Standalone firmware for validating a point-to-point LoRa link between two
LilyGO TTGO T3 v1.6.1 boards, as part of the WANADRI camera-trap image relay
project. This tests the radio, OLED status display, and payload integrity
independent of the Raspberry Pi gateway.

## Hardware

- 2x LilyGO TTGO T3 v1.6.1 (ESP32 + SX127x LoRa + SSD1306 OLED)
- Antenna on each board (required — never power on without one)
- USB cable per board for flashing/serial

## Pin Mapping

| Function | Pin |
|---|---|
| LoRa SCK | 5 |
| LoRa MISO | 19 |
| LoRa MOSI | 27 |
| LoRa SS (NSS) | 18 |
| LoRa RST | 14 |
| LoRa DIO0 | 26 |
| LoRa Band | 915E6 |
| OLED SDA | 21 |
| OLED SCL | 22 |
| OLED Reset | none (software reset, `-1`) |

> **Note:** earlier firmware used SDA=4 / SCL=15 and toggled GPIO16 as the
> OLED reset pin — both caused a `TG1WDT_SYS_RESET` boot loop on this board
> revision. SDA=21 / SCL=22 (per datasheet) with no reset-pin toggle fixed it.

## Files

- `lilygo_tx.ino` — sender. Broadcasts `HELLO#<n>|<crc32>` every 2 seconds.
- `lilygo_rx.ino` — receiver. Recomputes the CRC32 on each packet, logs
  RSSI/SNR, and flags corrupt payloads instead of trusting raw text.

## Dependencies (Arduino IDE Library Manager)

- `LoRa` by Sandeep Mistry
- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `Adafruit BusIO`

Board: **TTGO LoRa32-OLED** (ESP32 core by Espressif Systems).

## Flashing

1. Open `lilygo_tx.ino` in Arduino IDE, select board **TTGO LoRa32-OLED** and
   the correct port, upload.
2. Repeat with `lilygo_rx.ino` on the second board.
3. Open Serial Monitor at **115200 baud** on the RX board to watch results.

## Known Issue Log

| Symptom | Cause | Fix |
|---|---|---|
| Infinite `TG1WDT_SYS_RESET` reboot loop, OLED never lights up | Wrong I2C pins (SDA=4, SCL=15) | Use SDA=21, SCL=22 |
| `Guru Meditation Error: StoreProhibited` on boot | `display.clearDisplay()` called before `display.begin()` | Call `clearDisplay()` only after `begin()` succeeds |
| Board crash when toggling OLED reset | `pinMode`/`digitalWrite` on GPIO16 | Use software reset: `Adafruit_SSD1306 display(128, 64, &Wire, -1)` |

## Test Results (close range, indoor)

- Packets valid (CRC match): **100%**
- Packets lost: **0**
- RSSI: **-11 to -20 dBm**
- SNR: **~9.5 dB**

CRC32 validation confirms received payloads are bit-exact, not just visually
similar text.

## Next Steps

- Log RSSI / SNR / packet loss at increasing distances
- Merge back into the full `lilygo.cpp` firmware (LoRa + OLED + UART to Pi)
- Integrate with Raspberry Pi 5 gateway once USB boot media is available
