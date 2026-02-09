#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"
#include "sensor_direct.h"
#include "sensor_ms2.h"

// ========== DATA MANAGER CLASS ==========

class DataManager {
private:
  SensorDirect direct;
  SensorMS2 ms2;
  ValueData data[VALUE_CONFIG_COUNT];
  bool testMode;
  
  // Test mode variables
  float testStep;
  bool testUp;
  unsigned long lastTestUpdate;

  // Inicializar datos
  void initData() {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      data[i].current = 0.0;
      data[i].min = 999.0;
      data[i].max = -999.0;
      data[i].lastUpdate = 0;
      data[i].valid = false;
    }
    testStep = 0.0;
    testUp = true;
    lastTestUpdate = 0;
  }

  // Encontrar índice de un valor
  int getIndex(ValueType type) {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      if (VALUE_CONFIGS[i].type == type) {
        return i;
      }
    }
    return -1;
  }

  // Actualizar valores de test
  void updateTestValues() {
    if (millis() - lastTestUpdate < TEST_INTERVAL) return;
    lastTestUpdate = millis();

    testStep += testUp ? 0.05 : -0.05;
    if (testStep >= 1.0) testUp = false;
    if (testStep <= 0.0) testUp = true;

    // Generar valores test para cada tipo
    setValue(VALUE_MAP, -0.6 + testStep * 1.8);
    setValue(VALUE_OIL_PRESSURE, 1.0 + testStep * 4.0);
    setValue(VALUE_OIL_TEMP, 60.0 + testStep * 50.0);
    setValue(VALUE_AIR_TEMP, 10.0 + testStep * 60.0);
    setValue(VALUE_BATTERY, 12.0 + testStep * 1.8);
    setValue(VALUE_RPM, 800.0 + testStep * 6000.0);
    setValue(VALUE_TPS, testStep * 100.0);
    setValue(VALUE_COOLANT_TEMP, 70.0 + testStep * 40.0);
    setValue(VALUE_AFR, 12.0 + testStep * 5.0);
    setValue(VALUE_IGNITION, 10.0 + testStep * 30.0);
  }

  // Actualizar valores reales desde sensores
  void updateRealValues() {
    // Actualizar MS2 primero
    ms2.update();

    // Leer cada valor según su fuente
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      ValueType type = VALUE_CONFIGS[i].type;
      DataSource source = VALUE_CONFIGS[i].source;
      float value = 0.0;
      bool valid = false;

      if (source == SOURCE_DIRECT) {
        value = direct.read(type);
        valid = true;
      } 
      else if (source == SOURCE_MS2) {
        value = ms2.read(type);
        valid = ms2.isValid();
      }

      if (valid) {
        setValue(type, value);
      }
    }
  }

  // Establecer valor y actualizar min/max
  void setValue(ValueType type, float value) {
    int idx = getIndex(type);
    if (idx < 0) return;

    data[idx].current = value;
    data[idx].min = min(data[idx].min, value);
    data[idx].max = max(data[idx].max, value);
    data[idx].lastUpdate = millis();
    data[idx].valid = true;
  }

public:
  // Inicializar
  void begin(bool testModeEnabled = TEST_MODE_DEFAULT) {
    testMode = testModeEnabled;
    direct.begin();
    ms2.begin();
    initData();
  }

  // Actualizar - llamar en loop
  void update() {
    if (testMode) {
      updateTestValues();
    } else {
      updateRealValues();
    }
  }

  // Obtener valor actual
  float getValue(ValueType type) {
    int idx = getIndex(type);
    if (idx < 0) return 0.0;
    return data[idx].current;
  }

  // Obtener valor mínimo
  float getMin(ValueType type) {
    int idx = getIndex(type);
    if (idx < 0) return 0.0;
    return data[idx].min;
  }

  // Obtener valor máximo
  float getMax(ValueType type) {
    int idx = getIndex(type);
    if (idx < 0) return 0.0;
    return data[idx].max;
  }

  // Verificar si valor es válido
  bool isValid(ValueType type) {
    int idx = getIndex(type);
    if (idx < 0) return false;
    return data[idx].valid;
  }

  // Reset peaks (min/max)
  void resetPeaks() {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      data[i].min = 999.0;
      data[i].max = -999.0;
    }
  }

  // Toggle test mode
  void setTestMode(bool enabled) {
    testMode = enabled;
    if (enabled) {
      testStep = 0.0;
      testUp = true;
    }
  }

  // Get test mode status
  bool isTestMode() {
    return testMode;
  }

  // Obtener configuración de un valor
  const ValueConfig* getConfig(ValueType type) {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      if (VALUE_CONFIGS[i].type == type) {
        return &VALUE_CONFIGS[i];
      }
    }
    return nullptr;
  }
};

#endif
