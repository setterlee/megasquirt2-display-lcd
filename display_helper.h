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

  // Ya no hay conversión - cada sensor usa su unidad de VALUE_CONFIGS
  float convertValue(float value, Unit unit) {
    return value; // Retornar tal cual, sin conversión
  }

  // Obtener label de unidad según VALUE_CONFIGS
  const char* getUnitLabel(Unit unit) {
    switch (unit) {
      case UNIT_PSI:        return "p";
      case UNIT_BAR:        return "b";
      case UNIT_KPA:        return "k";
      case UNIT_CELSIUS:    return "C";
      case UNIT_FAHRENHEIT: return "F";
      case UNIT_VOLT:       return "V";
      case UNIT_RPM:        return "";
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

  // Versión compacta para mostrar 2 valores por línea (sin unidades)
  void printPeaksCompact(const DisplayItem& item) {
    if (item.value == VALUE_NONE) return;

    const ValueConfig* config = dataManager->getConfig(item.value);
    if (!config) return;

    float minVal = dataManager->getMin(item.value);
    float maxVal = dataManager->getMax(item.value);
    
    minVal = convertValue(minVal, config->unit);
    maxVal = convertValue(maxVal, config->unit);

    // Formato compacto: solo números, sin unidades
    // Ajustar decimales para ahorrar espacio
    uint8_t decimals = config->decimals;
    if (maxVal >= 100) decimals = 0;  // Si es >100, sin decimales
    
    lcd->print(minVal, decimals);
    lcd->print("/");
    lcd->print(maxVal, decimals);
  }

public:
  // Constructor
  DisplayHelper(LiquidCrystal_I2C* lcdPtr, DataManager* dmPtr) {
    lcd = lcdPtr;
    dataManager = dmPtr;
  }

  // Inicializar LCD
  void begin() {
    lcd->init();
    lcd->backlight();
  }

  // Mostrar mensaje de boot estilo Corvette C6
  void showBoot() {
    lcd->clear();
    
    // ========== "LINEA 1" - EFECTO LÍNEA Y CONSTRUCCIÓN ==========
    const char* lancer = "MITSUBISHI";
    int lancerLen = strlen(lancer);
    int startPos = (16 - lancerLen) / 2;  // Posición centrada
    
    // Dibujar línea inicial
    lcd->setCursor(startPos, 0);
    for (int i = 0; i < lancerLen; i++) {
      lcd->print("_");
    }
    delay(500);
    
    // Construir letra por letra reemplazando el guión
    for (int i = 0; i < lancerLen; i++) {
      lcd->setCursor(startPos + i, 0);
      lcd->print(lancer[i]);
      delay(150);
    }
    delay(600);
    
    // ========== "LINEA 2" - EFECTO LÍNEA Y CONSTRUCCIÓN ==========
    lcd->clear();
    
    const char* mitsubishi = "LANCER";
    int mitsuLen = strlen(mitsubishi);
    int mitsuStartPos = (16 - mitsuLen) / 2;
    
    // Dibujar línea inicial
    lcd->setCursor(mitsuStartPos, 0);
    for (int i = 0; i < mitsuLen; i++) {
      lcd->print("_");
    }
    delay(500);
    
    // Construir letra por letra reemplazando el guión
    for (int i = 0; i < mitsuLen; i++) {
      lcd->setCursor(mitsuStartPos + i, 0);
      lcd->print(mitsubishi[i]);
      delay(150);
    }
    delay(600);
    
    // ========== MENSAJE - CONSTRUIR LÍNEA POR LÍNEA ==========
    lcd->clear();
    
    const char* msg1 = "Hola Setterlee!";
    const char* msg2 = "Bienvenido...";
    
    int msg1Len = strlen(msg1);
    int msg1StartPos = (16 - msg1Len) / 2;
    
    // Línea 1 - dibujar línea base
    lcd->setCursor(msg1StartPos, 0);
    for (int i = 0; i < msg1Len; i++) {
      lcd->print("_");
    }
    delay(400);
    
    // Construir letra por letra
    for (int i = 0; i < msg1Len; i++) {
      lcd->setCursor(msg1StartPos + i, 0);
      lcd->print(msg1[i]);
      delay(80);
    }
    
    delay(400);
    
    int msg2Len = strlen(msg2);
    int msg2StartPos = (16 - msg2Len) / 2;
    
    // Línea 2 - dibujar línea base
    lcd->setCursor(msg2StartPos, 1);
    for (int i = 0; i < msg2Len; i++) {
      lcd->print("_");
    }
    delay(400);
    
    // Construir letra por letra
    for (int i = 0; i < msg2Len; i++) {
      lcd->setCursor(msg2StartPos + i, 1);
      lcd->print(msg2[i]);
      delay(80);
    }
    
    delay(1200);
    
    // ========== INFO DEL SISTEMA - PANTALLA ÚNICA ==========
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Alerts: "));
#if ENABLE_ALERTS
    lcd->print(F("ON"));
#else
    lcd->print(F("OFF"));
#endif
    
    lcd->setCursor(0, 1);
    lcd->print(F("Flags: "));
#if ENABLE_FLAG_NOTIFICATIONS
    lcd->print(F("ON"));
#else
    lcd->print(F("OFF"));
#endif
    
    delay(2000);
    lcd->clear();
  }

  // Renderizar una página normal
  void renderPage(uint8_t pageIndex) {
    if (pageIndex >= PAGE_COUNT) return;

    PageConfig page;
    memcpy_P(&page, &PAGES[pageIndex], sizeof(PageConfig));
    
    // Línea 1 - Item 1 en columna 0, Item 2 en columna 9
    lcd->setCursor(0, 0);
    printItem(page.line1.item1);
    lcd->print(F("         "));  // Limpiar hasta columna 9
    lcd->setCursor(9, 0);  // Posición fija para segundo valor
    printItem(page.line1.item2);
    lcd->print(F("       "));  // Limpiar resto de línea

    // Línea 2 - Item 1 en columna 0, Item 2 en columna 9
    lcd->setCursor(0, 1);
    printItem(page.line2.item1);
    lcd->print(F("         "));  // Limpiar hasta columna 9
    lcd->setCursor(9, 1);  // Posición fija para segundo valor
    printItem(page.line2.item2);
    lcd->print(F("       "));  // Limpiar resto de línea
  }

  // Renderizar peaks de una página
  void renderPeaks(uint8_t pageIndex) {
    if (pageIndex >= PAGE_COUNT) return;

    PageConfig page;
    memcpy_P(&page, &PAGES[pageIndex], sizeof(PageConfig));
    
    char labelBuf[5];
    
    // Línea 1: Mostrar ambos items (compacto)
    lcd->setCursor(0, 0);
    // Item 1 de línea 1
    if (page.line1.item1.value != VALUE_NONE) {
      const ValueConfig* cfg1 = dataManager->getConfig(page.line1.item1.value);
      strcpy_P(labelBuf, cfg1->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaksCompact(page.line1.item1);
    }
    // Item 2 de línea 1
    if (page.line1.item2.value != VALUE_NONE) {
      const ValueConfig* cfg2 = dataManager->getConfig(page.line1.item2.value);
      lcd->print(F(" "));
      strcpy_P(labelBuf, cfg2->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaksCompact(page.line1.item2);
    }

    // Línea 2: Mostrar ambos items (compacto)
    lcd->setCursor(0, 1);
    // Item 1 de línea 2
    if (page.line2.item1.value != VALUE_NONE) {
      const ValueConfig* cfg1 = dataManager->getConfig(page.line2.item1.value);
      strcpy_P(labelBuf, cfg1->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaksCompact(page.line2.item1);
    }
    // Item 2 de línea 2
    if (page.line2.item2.value != VALUE_NONE) {
      const ValueConfig* cfg2 = dataManager->getConfig(page.line2.item2.value);
      lcd->print(F(" "));
      strcpy_P(labelBuf, cfg2->label);
      lcd->print(labelBuf);
      lcd->print(F(" "));
      printPeaksCompact(page.line2.item2);
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

  // Toggle unit - ahora solo muestra info, no convierte
  void toggleUnit() {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("UNITS:"));
    lcd->setCursor(0, 1);
    lcd->print(F("From configs"));
  }

  // Clear display
  void clear() {
    lcd->clear();
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
  
  // Mostrar notificación de cambio de flag
  void showFlagNotification(ValueType flagType, bool newState) {
    lcd->clear();
    
    const ValueConfig* config = dataManager->getConfig(flagType);
    if (!config) return;
    
    // Buffer para leer label desde PROGMEM
    char labelBuf[5];
    strcpy_P(labelBuf, config->label);
    
    // Línea 1: nombre del flag con ":"
    lcd->setCursor(0, 0);
    lcd->print(labelBuf);
    lcd->print(F(":"));
    
    // Línea 2: transición de estado
    lcd->setCursor(0, 1);
    if (newState) {
      lcd->print(F("OFF -> ON"));
    } else {
      lcd->print(F("ON -> OFF"));
    }
  }
};

#endif
