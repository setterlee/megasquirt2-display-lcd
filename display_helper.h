#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "data_types.h"
#include "config.h"
#include "data_manager.h"

// ========== DISPLAY HELPER CLASS ==========

class DisplayHelper {
private:
  LiquidCrystal_I2C* lcd;
  DataManager* dataManager;
  bool usePSI;

  // Convertir valor según unidad seleccionada
  float convertValue(float value, Unit unit) {
    if (unit == UNIT_BAR && usePSI) {
      // Convertir BAR a PSI
      return value * 14.5038;
    }
    if (unit == UNIT_PSI && !usePSI) {
      // Convertir PSI a BAR
      return value / 14.5038;
    }
    return value;
  }

  // Obtener label de unidad
  const char* getUnitLabel(Unit unit) {
    if (unit == UNIT_BAR || unit == UNIT_PSI) {
      return usePSI ? "p" : "b";
    }
    switch (unit) {
      case UNIT_CELSIUS:    return "C";
      case UNIT_FAHRENHEIT: return "F";
      case UNIT_VOLT:       return "V";
      case UNIT_RPM:        return "r";
      case UNIT_PERCENT:    return "%";
      case UNIT_DEGREES:    return "d";
      case UNIT_MS:         return "m";
      case UNIT_RATIO:      return "";
      default:              return "";
    }
  }

  // Formatear y mostrar un item
  void printItem(const DisplayItem& item) {
    if (item.value == VALUE_NONE) return;

    const ValueConfig* config = dataManager->getConfig(item.value);
    if (!config) return;

    float value = dataManager->getValue(item.value);
    
    // Buffer para leer label desde PROGMEM
    char labelBuf[5];
    strcpy_P(labelBuf, config->label);
    
    // Caso especial: Engine flags individuales - mostrar como ON/OFF
    if (item.value >= VALUE_ENGINE_READY && item.value <= VALUE_ENGINE_FLATSHIFT) {
      lcd->print(labelBuf);
      lcd->print(F(":"));
      lcd->print(value > 0.5 ? F("ON ") : F("OFF"));
      return;
    }
    
    value = convertValue(value, config->unit);

    // Label
    lcd->print(labelBuf);
    lcd->print(F(":"));

    // Signo
    if (item.showSign && value >= 0) {
      lcd->print("+");
    }

    // Valor
    lcd->print(value, config->decimals);

    // Unidad
    if (item.showUnit) {
      lcd->print(getUnitLabel(config->unit));
    }
  }

  // Formatear y mostrar peaks
  void printPeaks(const DisplayItem& item) {
    if (item.value == VALUE_NONE) return;

    const ValueConfig* config = dataManager->getConfig(item.value);
    if (!config) return;

    float minVal = dataManager->getMin(item.value);
    float maxVal = dataManager->getMax(item.value);
    
    minVal = convertValue(minVal, config->unit);
    maxVal = convertValue(maxVal, config->unit);

    lcd->print(minVal, config->decimals);
    lcd->print("/");
    lcd->print(maxVal, config->decimals);

    if (item.showUnit) {
      lcd->print(getUnitLabel(config->unit));
    }
  }

public:
  // Constructor
  DisplayHelper(LiquidCrystal_I2C* lcdPtr, DataManager* dmPtr) {
    lcd = lcdPtr;
    dataManager = dmPtr;
    usePSI = USE_PSI_DEFAULT;
  }

  // Inicializar LCD
  void begin() {
    lcd->init();
    lcd->backlight();
  }

  // Mostrar mensaje de boot
  void showBoot() {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(BOOT_MSG_LINE1);
    lcd->setCursor(0, 1);
    lcd->print(BOOT_MSG_LINE2);
    delay(BOOT_DELAY);
    lcd->clear();
  }

  // Renderizar una página normal
  void renderPage(uint8_t pageIndex) {
    if (pageIndex >= PAGE_COUNT) return;

    PageConfig page;
    memcpy_P(&page, &PAGES[pageIndex], sizeof(PageConfig));
    
    // Línea 1
    lcd->setCursor(0, 0);
    printItem(page.line1.item1);
    lcd->print(F(" "));
    printItem(page.line1.item2);
    lcd->print(F("  ")); // Clear trailing chars

    // Línea 2
    lcd->setCursor(0, 1);
    printItem(page.line2.item1);
    lcd->print(F(" "));
    printItem(page.line2.item2);
    lcd->print(F("  ")); // Clear trailing chars
  }

  // Renderizar peaks de una página
  void renderPeaks(uint8_t pageIndex) {
    if (pageIndex >= PAGE_COUNT) return;

    PageConfig page;
    memcpy_P(&page, &PAGES[pageIndex], sizeof(PageConfig));
    
    char labelBuf[5];
    
    // Línea 1
    lcd->setCursor(0, 0);
    if (page.line1.item1.value != VALUE_NONE) {
      const ValueConfig* cfg = dataManager->getConfig(page.line1.item1.value);
      strcpy_P(labelBuf, cfg->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaks(page.line1.item1);
    }

    // Línea 2
    lcd->setCursor(0, 1);
    if (page.line2.item1.value != VALUE_NONE) {
      const ValueConfig* cfg = dataManager->getConfig(page.line2.item1.value);
      strcpy_P(labelBuf, cfg->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaks(page.line2.item1);
    }
  }

  // Mostrar mensaje temporal
  void showMessage(const __FlashStringHelper* line1, const __FlashStringHelper* line2, unsigned long duration = 0) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(line1);
    lcd->setCursor(0, 1);
    lcd->print(line2);
    
    if (duration > 0) {
      delay(duration);
      lcd->clear();
    }
  }

  // Toggle unit (BAR <-> PSI)
  void toggleUnit() {
    usePSI = !usePSI;
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("UNIT CHANGE:"));
    lcd->setCursor(0, 1);
    lcd->print(usePSI ? F("BAR -> PSI") : F("PSI -> BAR"));
  }

  // Clear display
  void clear() {
    lcd->clear();
  }

  // Get current unit mode
  bool isPSI() {
    return usePSI;
  }
  
  // Control de backlight
  void setBacklight(bool on) {
    if (on) {
      lcd->backlight();
    } else {
      lcd->noBacklight();
    }
  }
  
  // Mostrar alerta en pantalla
  void showAlert(const char* line1, const char* line2) {
    lcd->clear();
    
    if (line1) {
      lcd->setCursor(0, 0);
      // Leer desde PROGMEM
      char buf[17];
      strcpy_P(buf, line1);
      lcd->print(buf);
    }
    
    if (line2) {
      lcd->setCursor(0, 1);
      // Leer desde PROGMEM
      char buf[17];
      strcpy_P(buf, line2);
      lcd->print(buf);
    }
  }
};

#endif
