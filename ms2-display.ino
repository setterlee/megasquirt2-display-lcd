// ========================================
// LANCER MONITOR - Modular Architecture
// ========================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Incluir módulos del sistema
#include "data_types.h"
#include "config.h"
#include "sensor_direct.h"
#include "sensor_ms2.h"
#include "data_manager.h"
#include "alert_manager.h"
#include "display_helper.h"

// ========== GLOBAL OBJECTS ==========

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
DataManager dataManager;
AlertManager alertManager(&dataManager);
DisplayHelper display(&lcd, &dataManager);

// ========== STATE ==========

uint8_t currentPage = 0;
bool showPeaks = false;
bool showUnitChange = false;

unsigned long lastUpdate = 0;
unsigned long peakViewStart = 0;
unsigned long unitChangeStart = 0;

// ========== SETUP ==========

void setup() {
  // Inicializar botones
  pinMode(BTN_PAGE, INPUT_PULLUP);
  pinMode(BTN_PEAK, INPUT_PULLUP);

  // Inicializar I2C
  Wire.begin();

  // Inicializar subsistemas
  dataManager.begin(TEST_MODE_DEFAULT);
  display.begin();

  // Mostrar mensaje de boot
  display.showBoot();
}

// ========== LOOP ==========

void loop() {
  handlePageButton();
  handlePeakButton();

  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();

    // Actualizar datos
    dataManager.update();
    
    // Verificar alertas
    alertManager.update();
    
    // Controlar backlight según alertas
    if (alertManager.shouldBlink()) {
      display.setBacklight(true);
    } else {
      display.setBacklight(false);
    }

    // Manejar timeout de peaks
    if (showPeaks && millis() - peakViewStart > PEAK_VIEW_TIME) {
      showPeaks = false;
      display.clear();
    }

    // Manejar timeout de mensaje de unidad
    if (showUnitChange && millis() - unitChangeStart > UNIT_MSG_TIME) {
      showUnitChange = false;
      display.clear();
    }

    // Renderizar pantalla
    if (!showUnitChange) {
      if (alertManager.isAlertActive()) {
        // Mostrar alerta en lugar de la página
        display.showAlert(alertManager.getAlertLine1(), alertManager.getAlertLine2());
      }
      else if (showPeaks) {
        display.renderPeaks(currentPage);
      } else {
        display.renderPage(currentPage);
      }
    }
  }
}

// ========== BUTTON HANDLERS ==========

void handlePageButton() {
  static bool lastState = HIGH;
  static unsigned long pressStart = 0;
  static bool longHandled = false;
  bool current = digitalRead(BTN_PAGE);

  if (lastState == HIGH && current == LOW) {
    pressStart = millis();
    longHandled = false;
  }

  // Long press: cambiar unidad
  if (current == LOW && !longHandled &&
      millis() - pressStart >= LONG_PRESS_TIME) {
    display.toggleUnit();
    showUnitChange = true;
    unitChangeStart = millis();
    longHandled = true;
  }

  // Short press: cambiar página
  if (lastState == LOW && current == HIGH && !longHandled) {
    currentPage = (currentPage + 1) % PAGE_COUNT;
    display.clear();
  }

  lastState = current;
}

void handlePeakButton() {
  static bool lastState = HIGH;
  static unsigned long pressStart = 0;
  static bool longHandled = false;

  bool current = digitalRead(BTN_PEAK);

  if (lastState == HIGH && current == LOW) {
    pressStart = millis();
    longHandled = false;
  }

  // Long press: reset peaks
  if (current == LOW && !longHandled &&
      millis() - pressStart >= LONG_PRESS_TIME) {
    dataManager.resetPeaks();
    display.showMessage(F("PEAK RESET"), F("DONE"), 1000);
    longHandled = true;
  }

  // Short press: mostrar peaks
  if (lastState == LOW && current == HIGH && !longHandled) {
    showPeaks = true;
    peakViewStart = millis();
    display.clear();
  }

  lastState = current;
}