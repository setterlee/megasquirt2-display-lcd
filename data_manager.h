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
      ValueConfig cfg;
      memcpy_P(&cfg, &VALUE_CONFIGS[i], sizeof(ValueConfig));
      if (cfg.type == type) {
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

    // Generar valores test para cada tipo (rangos realistas)
    setValue(VALUE_MAP, -8.7 + testStep * 26.1);           // -8.7 a +17.4 PSI (vacío a boost)
    setValue(VALUE_OIL_PRESSURE, 14.5 + testStep * 58.5);  // 14.5 a 73.0 PSI (ralentí a alta)
    setValue(VALUE_OIL_TEMP, 60.0 + testStep * 50.0);      // 60 a 110°C (normal a alta)
    setValue(VALUE_AIR_TEMP, 10.0 + testStep * 40.0);      // 10 a 50°C (frío a caliente)
    setValue(VALUE_BATTERY, 12.5 + testStep * 2.0);        // 12.5 a 14.5V (descarga a carga)
    setValue(VALUE_RPM, 800.0 + testStep * 6000.0);        // 800 a 6800 RPM
    setValue(VALUE_TPS, testStep * 100.0);                 // 0 a 100%
    setValue(VALUE_COOLANT_TEMP, 70.0 + testStep * 30.0);  // 70 a 100°C (frío a caliente)
    setValue(VALUE_AFR, 12.0 + testStep * 5.0);            // 12.0 a 17.0 AFR (rico a pobre)
    setValue(VALUE_IGNITION, 10.0 + testStep * 25.0);      // 10 a 35° (ralentí a máximo)
    setValue(VALUE_FUEL_PRESSURE, 35.0 + testStep * 15.0); // 35 a 50 PSI (ralentí a máximo)
    setValue(VALUE_PULSE_WIDTH, 2.0 + testStep * 8.0);     // 2 a 10 ms (ralentí a carga)
    
    // Engine status flags individuales - ciclar para testing
    setValue(VALUE_ENGINE_READY, 1.0);                      // Siempre activo en test
    setValue(VALUE_ENGINE_CRANK, testStep < 0.1 ? 1.0 : 0.0);  // Solo al inicio
    setValue(VALUE_ENGINE_ASE, testStep < 0.3 ? 1.0 : 0.0);    // Primeros instantes
    setValue(VALUE_ENGINE_WARMUP, testStep < 0.5 ? 1.0 : 0.0); // Calentamiento
    setValue(VALUE_ENGINE_TPS_AE, testStep > 0.5 ? 1.0 : 0.0); // Aceleración
    setValue(VALUE_ENGINE_LAUNCH, testStep > 0.8 ? 1.0 : 0.0); // Alta carga
    setValue(VALUE_ENGINE_FLATSHIFT, 0.0);                      // Raramente activo
  }

  // Actualizar valores reales desde sensores
  void updateRealValues() {
    // Actualizar MS2 primero
    ms2.update();

    // Leer cada valor según su fuente
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      ValueConfig cfg;
      memcpy_P(&cfg, &VALUE_CONFIGS[i], sizeof(ValueConfig));
      
      ValueType type = cfg.type;
      DataSource source = cfg.source;
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

  // Obtener configuración de un valor (retorna copia desde PROGMEM)
  static ValueConfig getConfigCopy(ValueType type) {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      ValueConfig cfg;
      memcpy_P(&cfg, &VALUE_CONFIGS[i], sizeof(ValueConfig));
      if (cfg.type == type) {
        return cfg;
      }
    }
    // Retornar config vacía si no se encuentra
    ValueConfig empty = {VALUE_NONE, SOURCE_TEST, nullptr, UNIT_PSI, 0, 0, 0, 0.0, 0.0, 0};
    return empty;
  }
  
  // Obtener configuración de un valor (versión pointer para compatibilidad)
  const ValueConfig* getConfig(ValueType type) {
    static ValueConfig tempCfg;
    tempCfg = getConfigCopy(type);
    if (tempCfg.type == VALUE_NONE) return nullptr;
    return &tempCfg;
  }
};

#endif
