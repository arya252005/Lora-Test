# WANADRI — Tes LoRa TX/RX LilyGO T3

Firmware standalone buat validasi link LoRa point-to-point antar 2 board
LilyGO TTGO T3 v1.6.1, bagian dari project WANADRI (relay foto camera-trap
lewat LoRa mesh). Ini nguji radio, OLED status display, dan integritas
payload, terpisah dari gateway Raspberry Pi.

## Hardware

- 2x LilyGO TTGO T3 v1.6.1 (ESP32 + SX127x LoRa + SSD1306 OLED)
- Antena tiap board (WAJIB — jangan nyalain tanpa antena)
- Kabel USB per board buat flash/serial

## Pin Mapping

| Fungsi | Pin |
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
| OLED Reset | gak dipake (software reset, `-1`) |

> **Catatan:** firmware awal pake SDA=4 / SCL=15 dan toggle GPIO16 sbg reset
> pin OLED — dua-duanya bikin boot loop `TG1WDT_SYS_RESET` di revisi board
> ini. SDA=21 / SCL=22 (sesuai datasheet) tanpa toggle reset pin, fix.

## File

- `lilygo_tx.ino` — pengirim. Broadcast `HELLO#<n>|<crc32>` tiap 2 detik.
- `lilygo_rx.ino` — penerima. Hitung ulang CRC32 tiap paket, log RSSI/SNR,
  tandain payload korup, gak asal percaya teks mentah.

## Dependency (Arduino IDE Library Manager)

- `LoRa` by Sandeep Mistry
- `Adafruit SSD1306`
- `Adafruit GFX Library`
- `Adafruit BusIO`

Board: **TTGO LoRa32-OLED** (ESP32 core by Espressif Systems).

## Cara Flash

1. Buka `lilygo_tx.ino` di Arduino IDE, pilih board **TTGO LoRa32-OLED**
   dan port yg bener, Upload.
2. Ulang buat `lilygo_rx.ino` di board kedua.
3. Buka Serial Monitor baud **115200** di board RX buat liat hasil.

## Log Bug yg Ketemu

| Gejala | Penyebab | Fix |
|---|---|---|
| Reboot loop `TG1WDT_SYS_RESET` terus-terusan, OLED gak nyala | Pin I2C salah (SDA=4, SCL=15) | Pake SDA=21, SCL=22 |
| `Guru Meditation Error: StoreProhibited` pas boot | `display.clearDisplay()` dipanggil sebelum `display.begin()` | Panggil `clearDisplay()` cuma setelah `begin()` sukses |
| Board crash pas toggle reset OLED | `pinMode`/`digitalWrite` di GPIO16 | Pake software reset: `Adafruit_SSD1306 display(128, 64, &Wire, -1)` |

## Hasil Tes (jarak deket, indoor)

- Paket valid (CRC cocok): **100%**
- Paket hilang: **0**
- RSSI: **-11 sampe -20 dBm**
- SNR: **~9.5 dB**

Validasi CRC32 mastiin payload yg diterima bener-bener sama persis bit demi
bit, bukan cuma keliatan mirip doang.

## Next Steps

- Log RSSI / SNR / packet loss di jarak makin jauh
- Gabung balik ke firmware lengkap `lilygo.cpp` (LoRa + OLED + UART ke Pi)
- Integrasi ke gateway Raspberry Pi 5 abis storage USB-nya kelar
