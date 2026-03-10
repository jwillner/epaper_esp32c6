# ESP32-C6-Zero + Waveshare 1.54" ePaper

Ein Arduino-Projekt für das **Waveshare ESP32-C6-Zero** Board mit einem **1.54" ePaper Display (200×200, SW/W)**.

---

## Hardware

| Komponente | Modell |
|-----------|--------|
| MCU Board | Waveshare ESP32-C6-Zero |
| Display   | Waveshare 1.54" e-Paper Module (200×200, BW) |

---

## Verdrahtung

| Display Pin | ESP32-C6-Zero GPIO |
|-------------|-------------------|
| VCC         | 3V3               |
| GND         | GND               |
| DIN (MOSI)  | GPIO5             |
| CLK (SCK)   | GPIO4             |
| CS          | GPIO21            |
| DC          | GPIO3             |
| RST         | GPIO2             |
| BUSY        | GPIO1             |

> **Wichtig:** GPIO9 ist ein Strapping-Pin auf dem ESP32-C6 und darf **nicht** als CS verwendet werden (s. unten).

---

## Software

- Framework: Arduino (via arduino-cli)
- Board FQBN: `esp32:esp32:esp32c6`
- Bibliothek: [GxEPD2](https://github.com/ZinggJM/GxEPD2)
- Treiber: `GxEPD2_154_GDEY0154D67` (SSD1681 Controller)

---

## Build & Upload

```bash
# Kompilieren
arduino-cli compile --fqbn esp32:esp32:esp32c6 epaper_test

# Port ermitteln
arduino-cli board list

# Hochladen
arduino-cli upload -p /dev/ttyACM0 --fqbn esp32:esp32:esp32c6 epaper_test

# Serieller Monitor
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
```

---

## Projekt-Geschichte: Was haben wir herausgefunden?

Dieses Projekt entstand mit Hilfe von Claude Code. Der Weg zum ersten „Hello World" auf dem ePaper war überraschend steinig — hier sind die Learnings dokumentiert.

---

### Problem 1: Falscher Display-Treiber

**Symptom:** Display bleibt komplett weiß, keine Reaktion.

**Ursache:** GxEPD2 enthält mehrere Treiber für 1.54" Displays. Der naheliegende `GxEPD2_154_D67` passte **nicht** zum verbauten Controller.

**Lösung:** Der korrekte Treiber ist:
```cpp
#include <gdey/GxEPD2_154_GDEY0154D67.h>
```
Der Controller ist ein **SSD1681**, der unter `gdey/GxEPD2_154_GDEY0154D67` liegt — nicht in der Hauptebene der Bibliothek.

---

### Problem 2: GPIO9 ist ein Strapping-Pin!

**Symptom:** Board bootet nicht korrekt, Sketch läuft nicht an, oder verhält sich zufällig.

**Ursache:** Auf dem ESP32-C6 werden beim Boot bestimmte GPIO-Pins als **Strapping-Pins** gelesen, um den Boot-Modus festzulegen. GPIO9 gehört dazu.

Wenn CS auf GPIO9 liegt und beim Reset LOW ist (weil SPI gerade aktiv war), interpretiert der ESP32-C6 das als „Boot in ROM Download Mode" → kein normaler Sketch-Start.

**Strapping-Pins ESP32-C6 (niemals für SPI/Display verwenden):**
- GPIO8
- GPIO9
- GPIO15

**Lösung:** CS auf **GPIO21** gelegt.

---

### Problem 3: GxEPD2 überschreibt Custom-SPI-Pins

**Symptom:** Mit dem „Standard"-Aufruf `SPI.begin()` + `display.init()` ignoriert GxEPD2 die eigenen Pin-Definitionen und fällt auf Default-Pins zurück.

**Ursache:** GxEPD2 initialisiert SPI intern neu, wenn kein eigenes `SPIClass`-Objekt übergeben wird.

**Lösung:** Eigene SPIClass-Instanz erstellen und explizit übergeben:
```cpp
SPIClass hspi(FSPI);
hspi.begin(4, -1, 5, 21); // SCK, MISO, MOSI, CS

display.init(115200, true, 50, false, hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
```

---

### Problem 4: CS und RST müssen sofort auf HIGH

**Symptom:** Zufälliges Fehlverhalten nach Reset, Display reagiert auf ersten Init nicht.

**Ursache:** Während des ESP32-Bootvorgangs sind die GPIO-Pins zunächst im Floating-Zustand. Wenn CS dabei LOW ist, empfängt der Display-Controller zufällige SPI-Daten und gerät in einen ungültigen Zustand.

**Lösung:** CS und RST als erstes im Code auf HIGH setzen, **bevor** SPI oder Display initialisiert werden:
```cpp
void setup() {
  pinMode(PIN_EPD_CS,  OUTPUT); digitalWrite(PIN_EPD_CS,  HIGH);
  pinMode(PIN_EPD_RST, OUTPUT); digitalWrite(PIN_EPD_RST, HIGH);
  delay(100);
  // ... dann erst SPI und display.init()
}
```

---

### Problem 5: USB-CDC Serial blockiert ohne Terminal

**Symptom:** Sketch läuft nicht an, wenn kein Serial Monitor geöffnet ist.

**Ursache:** Der ESP32-C6-Zero verwendet **USB-CDC** (Hardware-USB direkt im Chip). `Serial.begin()` wartet darauf, dass ein Host die CDC-Verbindung öffnet — wenn kein Terminal offen ist, hängt der Code ewig.

**Lösung:** Timeout beim Warten auf Serial:
```cpp
Serial.begin(115200);
uint32_t t = millis();
while (!Serial && millis() - t < 2000);
```

---

### Problem 6: Display hängt nach mehreren Uploads

**Symptom:** Nach dem zweiten oder dritten Upload bleibt das Display weiß oder reagiert nicht mehr.

**Ursache:** Der Display-Controller gerät durch den Neustart während einer laufenden Refresh-Sequenz in einen ungültigen internen Zustand.

**Lösung:** **Power-Cycle** (USB-Kabel abziehen und wieder einstecken). Ein einfacher Reset-Knopf reicht nicht — der Display-Controller braucht echten Stromausfall.

---

## Aktueller Stand des Sketches

Der Sketch zeigt:
- „Hello World!"
- „WeAct Studio"
- Einen Counter, der alle 5 Sekunden hochzählt

```
Hello World!
WeAct Studio
Counter:
42
```

---

## Nächste Schritte (Ideen)

- [ ] DeepSleep zwischen Updates (Stromsparen)
- [ ] Sensorwerte anzeigen (Temperatur, Luftfeuchte)
- [ ] WLAN-Verbindung + Daten aus dem Internet
- [ ] MQTT / Home Assistant Integration
- [ ] Partial Refresh für schnellere Updates

---

## Lizenz

MIT
