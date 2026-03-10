# CLAUDE.md

## Projekt

ESP32-C6 + Waveshare 1.54" ePaper Display

Dieses Projekt verbindet ein Waveshare ePaper Display mit einem ESP32-C6-Zero Board und wird mit **arduino-cli** gebaut.

---

# Hardware

## Board

ESP32-C6-Zero (Waveshare)

USB-C Dev Board mit ESP32-C6.

---

## Display

Waveshare **1.54 inch e-Paper Module**

Eigenschaften

- Auflösung: 200 × 200
- Farben: schwarz / weiss
- Interface: SPI
- Pins: VCC, GND, DIN, CLK, CS, DC, RST, BUSY

---

# Verdrahtung

Display → ESP32-C6

VCC  -> 3V3
GND  -> GND

DIN  -> GPIO5
CLK  -> GPIO4

CS   -> GPIO21  (NICHT GPIO9 — ist Strapping-Pin auf ESP32-C6!)
DC   -> GPIO3
RST  -> GPIO2
BUSY -> GPIO1

---

# SPI Konfiguration

SPI wird manuell initialisiert:

SPI.begin(4, -1, 5, 9);

Parameter

SCK  = GPIO4
MISO = -1
MOSI = GPIO5
CS   = GPIO9

---

# Display Pins im Code

#define PIN_EPD_CS   21
#define PIN_EPD_DC   3
#define PIN_EPD_RST  2
#define PIN_EPD_BUSY 1

---

# Software Stack

Buildsystem

arduino-cli

Framework

Arduino (ESP32)

Board

esp32:esp32:esp32c6

Bibliothek

GxEPD2

---

# Build

arduino-cli compile --fqbn esp32:esp32:esp32c6 epaper_test

---

# Upload

arduino-cli board list

arduino-cli upload -p /dev/ttyACM0 --fqbn esp32:esp32:esp32c6 epaper_test

---

# Serial Monitor

arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200

---

# Verwendeter Displaytreiber

Primär:

GxEPD2_154_D67

Falls das Display nicht reagiert, testen:

GxEPD2_154
GxEPD2_154_D67
GxEPD2_154c

---

# Regeln für Claude Code

Claude Code soll:

1. Arduino kompatiblen Code erzeugen
2. kein PlatformIO verwenden
3. vorhandene Pinbelegung respektieren
4. GxEPD2 Bibliothek verwenden
5. SPI.begin(...) nicht entfernen
6. Änderungen erklären

---

# Typische Probleme

Display bleibt weiß

Mögliche Ursachen

- falscher Displaytreiber
- BUSY Pin nicht angeschlossen
- falsche SPI Pins

---

# Ziel des Projekts

Erstes Ziel

Hello-World auf dem ePaper anzeigen.

Später

- DeepSleep
- Sensorwerte anzeigen
- WLAN Daten anzeigen
- MQTT / Home Assistant
