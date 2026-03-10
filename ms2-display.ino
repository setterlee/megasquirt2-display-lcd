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
bool showFlagNotification = false;

// Marquee state
uint8_t marqueeIndex = 0;  // 0-3: cual valor está visible (line1.item1, line1.item2, line2.item1, line2.item2)
unsigned long lastMarqueeRotation = 0;

unsigned long lastUpdate = 0;
unsigned long peakViewStart = 0;
unsigned long unitChangeStart = 0;
unsigned long flagNotificationStart = 0;

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
    
#if ENABLE_FLAG_NOTIFICATIONS
    // Verificar si hay nueva notificación de flag
    if (dataManager.hasFlagNotification() && !showFlagNotification) {
      showFlagNotification = true;
      flagNotificationStart = millis();
      display.clear();
    }
#endif
    
#if ENABLE_ALERTS
    // Verificar alertas
    alertManager.update();
    
    // Controlar backlight según alertas (blink)
    if (alertManager.shouldBlink()) {
      display.setBacklight(true);
    } else {
      display.setBacklight(false);
    }
#else
    // Alertas desactivadas - backlight siempre encendido
    display.setBacklight(true);
#endif

    // Manejar timeout de peaks
    if (showPeaks && millis() - peakViewStart > PEAK_VIEW_TIME) {
      showPeaks = false;
      marqueeIndex = 0;  // Reset marquee
      display.clear();
    }
    
    // Manejar rotación de marquee cuando está en modo peak
    if (showPeaks && millis() - lastMarqueeRotation > MARQUEE_INTERVAL) {
      lastMarqueeRotation = millis();
      marqueeIndex = (marqueeIndex + 1) % 4;  // Rotar entre 0-3
      display.clear();
    }

    // Manejar timeout de mensaje de unidad
    if (showUnitChange && millis() - unitChangeStart > UNIT_MSG_TIME) {
      showUnitChange = false;
      display.clear();
    }
    
#if ENABLE_FLAG_NOTIFICATIONS
    // Manejar timeout de notificación de flag
    if (showFlagNotification && millis() - flagNotificationStart > FLAG_NOTIFICATION_TIME) {
      showFlagNotification = false;
      dataManager.clearFlagNotification();
      display.clear();
    }
#endif

    // Renderizar pantalla
    if (!showUnitChange) {
#if ENABLE_FLAG_NOTIFICATIONS
      // Prioridad 1: Notificación de flag
      if (showFlagNotification) {
        FlagNotification notif = dataManager.getFlagNotification();
        display.showFlagNotification(notif.flagType, notif.newState);
      }
      else
#endif
#if ENABLE_ALERTS
      if (alertManager.isAlertActive()) {
        // Mostrar alerta en lugar de la página
        display.showAlert(alertManager.getAlertLine1(), alertManager.getAlertLine2());
      }
      else
#endif
      if (showPeaks) {
        display.renderPeaksMarquee(currentPage, marqueeIndex);
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
    marqueeIndex = 0;  // Empezar desde primer valor
    lastMarqueeRotation = millis();
    display.clear();
  }

  lastState = current;
}