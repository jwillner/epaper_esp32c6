// ESP32-C6-Zero + Waveshare 1.54" ePaper (200x200)
// Treiber: GxEPD2_154_GDEY0154D67 (SSD1681)

#define ENABLE_GxEPD2_GFX 0

#include <SPI.h>
#include <GxEPD2_BW.h>
#include <gdey/GxEPD2_154_GDEY0154D67.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define PIN_EPD_CS    21
#define PIN_EPD_DC    3
#define PIN_EPD_RST   2
#define PIN_EPD_BUSY  1
#define PIN_SPI_SCK   4
#define PIN_SPI_MOSI  5

#define UPDATE_INTERVAL_MS 5000

SPIClass hspi(FSPI);

GxEPD2_BW<GxEPD2_154_GDEY0154D67, GxEPD2_154_GDEY0154D67::HEIGHT> display(
  GxEPD2_154_GDEY0154D67(PIN_EPD_CS, PIN_EPD_DC, PIN_EPD_RST, PIN_EPD_BUSY)
);

void showAll(uint32_t count) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%lu", count);

  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(10, 40);
    display.print("Hello World!");
    display.setCursor(10, 80);
    display.print("WeAct Studio");
    display.setCursor(10, 130);
    display.print("Counter:");
    display.setCursor(10, 170);
    display.print(buf);
  } while (display.nextPage());
}

void setup() {
  // CS und RST sofort auf definierten Zustand bringen
  // (verhindert floating während ESP32-Boot → zufällige SPI-Daten an Display)
  pinMode(PIN_EPD_CS,  OUTPUT); digitalWrite(PIN_EPD_CS,  HIGH);
  pinMode(PIN_EPD_RST, OUTPUT); digitalWrite(PIN_EPD_RST, HIGH);

  delay(100);

  hspi.begin(PIN_SPI_SCK, -1, PIN_SPI_MOSI, PIN_EPD_CS);
  display.init(115200, true, 50, false, hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));

  showAll(0);
}

void loop() {
  static uint32_t count = 0;
  static uint32_t lastUpdate = millis();

  if (millis() - lastUpdate >= UPDATE_INTERVAL_MS) {
    lastUpdate = millis();
    count++;
    showAll(count);
  }
}
