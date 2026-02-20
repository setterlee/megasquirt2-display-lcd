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
  
  // Flag tracking para notificaciones
  float previousFlagValues[10];  // Track previous values for engine flags
  FlagNotification currentNotification;

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
    
    // Inicializar tracking de flags
    for (uint8_t i = 0; i < 10; i++) {
      previousFlagValues[i] = 0.0;
    }
    currentNotification.active = false;
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

    testStep += testUp ? 0.005 : -0.005;  // Incremento más lento para ciclo de ~24 segundos
    if (testStep >= 1.0) testUp = false;
    if (testStep <= 0.0) testUp = true;

    // CASO DE PRUEBA 1: Motor frío + aceleración fuerte (testStep 0.0-0.2)
    // Simula: Motor recién arrancado, temperaturas bajas, usuario acelera
    if (testStep <= 0.2) {
      setValue(VALUE_COOLANT_TEMP, 65.0);              // Motor frío (< 85°C)
      setValue(VALUE_OIL_TEMP, 70.0);                  // Aceite frío (< 85°C)
      setValue(VALUE_TPS, 60.0 + testStep * 200);     // TPS sube a > 50%
      setValue(VALUE_MAP, 80.0 + testStep * 100);     // MAP 80-100 kPa (boost bajo)
      setValue(VALUE_OIL_PRESSURE, 25.0);
      setValue(VALUE_RPM, 2500.0);
      // ⚠️ ALERTA: COLD_ENGINE_HIGH_LOAD debe activarse
    }
    // CASO DE PRUEBA 2: Baja presión aceite + RPM altas (testStep 0.2-0.4)
    else if (testStep <= 0.4) {
      setValue(VALUE_OIL_PRESSURE, 8.0);               // Presión crítica (< 10 PSI)
      setValue(VALUE_RPM, 4000.0);                     // RPM altas (> 2000)
      setValue(VALUE_COOLANT_TEMP, 90.0);
      setValue(VALUE_OIL_TEMP, 100.0);
      setValue(VALUE_TPS, 30.0);
      setValue(VALUE_MAP, 50.0);                       // MAP 50 kPa (vacío parcial)
      // 🚨 ALERTA CRÍTICA: LOW_OIL_PRESSURE debe activarse
    }
    // CASO DE PRUEBA 3: Temperaturas combinadas altas (testStep 0.4-0.6)
    else if (testStep <= 0.6) {
      setValue(VALUE_COOLANT_TEMP, 110.0);             // Coolant crítico (> 105°C)
      setValue(VALUE_OIL_TEMP, 130.0);                 // Aceite crítico (> 120°C)
      setValue(VALUE_OIL_PRESSURE, 35.0);
      setValue(VALUE_RPM, 3500.0);
      setValue(VALUE_TPS, 45.0);
      setValue(VALUE_MAP, 70.0);                       // MAP 70 kPa (carga moderada)
      // 🚨 ALERTA CRÍTICA: HIGH_TEMP_COMBO debe activarse
    }
    // CASO DE PRUEBA 4: Operación normal (testStep 0.6-1.0)
    else {
      setValue(VALUE_COOLANT_TEMP, 85.0 + testStep * 10.0);   // 85-95°C normal
      setValue(VALUE_OIL_TEMP, 90.0 + testStep * 15.0);       // 90-105°C normal
      setValue(VALUE_OIL_PRESSURE, 25.0 + testStep * 30.0);   // 25-55 PSI normal
      setValue(VALUE_RPM, 1500.0 + testStep * 3000.0);        // 1500-4500 RPM
      setValue(VALUE_TPS, testStep * 40.0);                   // 0-40% TPS moderado
      setValue(VALUE_MAP, 40.0 + testStep * 100.0);           // 40-140 kPa (vacío a boost moderado)
      // ✅ Sin alertas: todo en rangos normales
    }

    // Valores comunes para todos los casos
    setValue(VALUE_AIR_TEMP, 20.0 + testStep * 30.0);
    setValue(VALUE_BATTERY, 13.5 + testStep * 1.0);
    setValue(VALUE_AFR, 13.5 + testStep * 2.0);
    setValue(VALUE_IGNITION, 15.0 + testStep * 20.0);
    setValue(VALUE_DWELL, 2.5 + testStep * 3.0);           // 2.5 a 5.5 ms (ralentí a alta carga)
    setValue(VALUE_FUEL_PRESSURE, 40.0 + testStep * 10.0);
    setValue(VALUE_PULSE_WIDTH, 3.0 + testStep * 6.0);
    
    // Engine status flags - simulación realista de secuencia de arranque y operación
    // READY: se activa después del arranque y queda ON
    setValue(VALUE_ENGINE_READY, testStep >= 0.05 ? 1.0 : 0.0);
    
    // CRANK: solo durante arranque inicial (primeros frames)
    setValue(VALUE_ENGINE_CRANK, (testStep >= 0.0 && testStep < 0.05) ? 1.0 : 0.0);
    
    // ASE (After Start Enrichment): activo brevemente después del arranque
    setValue(VALUE_ENGINE_ASE, (testStep >= 0.06 && testStep < 0.16) ? 1.0 : 0.0);
    
    // WARMUP: activo mientras el motor se calienta (más largo)
    setValue(VALUE_ENGINE_WARMUP, (testStep >= 0.17 && testStep < 0.48) ? 1.0 : 0.0);
    
    // TPS_AE (Acceleration Enrichment): solo durante aceleraciones fuertes
    // Simula dos pisadas de acelerador en momentos diferentes
    setValue(VALUE_ENGINE_TPS_AE, 
      ((testStep >= 0.21 && testStep < 0.26) || (testStep >= 0.72 && testStep < 0.77)) ? 1.0 : 0.0);
    
    // LAUNCH: solo en condición específica (RPM alta + condiciones)
    setValue(VALUE_ENGINE_LAUNCH, (testStep >= 0.83 && testStep < 0.88) ? 1.0 : 0.0);
    
    // FLATSHIFT: muy raro, solo un momento específico
    setValue(VALUE_ENGINE_FLATSHIFT, (testStep >= 0.93 && testStep < 0.96) ? 1.0 : 0.0);
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

  // Verificar si un tipo de valor es un flag
  bool isFlag(ValueType type) {
    return (type >= VALUE_ENGINE_READY && type <= VALUE_ENGINE_FLATSHIFT);
  }
  
  // Obtener índice de flag (0-9) para el array previousFlagValues
  int getFlagIndex(ValueType type) {
    if (!isFlag(type)) return -1;
    return type - VALUE_ENGINE_READY;
  }
  
  // Establecer valor y actualizar min/max
  void setValue(ValueType type, float value) {
    int idx = getIndex(type);
    if (idx < 0) return;

#if ENABLE_FLAG_NOTIFICATIONS
    // Detectar cambios en flags
    if (isFlag(type)) {
      int flagIdx = getFlagIndex(type);
      if (flagIdx >= 0) {
        float prevValue = previousFlagValues[flagIdx];
        bool prevState = (prevValue > 0.5);
        bool newState = (value > 0.5);
        
        // Si el flag cambió de estado, crear notificación
        if (data[idx].valid && prevState != newState) {
          currentNotification.flagType = type;
          currentNotification.newState = newState;
          currentNotification.timestamp = millis();
          currentNotification.active = true;
        }
        
        // Actualizar valor anterior
        previousFlagValues[flagIdx] = value;
      }
    }
#endif

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
  
  // Obtener notificación activa de flag
  bool hasFlagNotification() {
    return currentNotification.active;
  }
  
  FlagNotification getFlagNotification() {
    return currentNotification;
  }
  
  // Limpiar notificación de flag
  void clearFlagNotification() {
    currentNotification.active = false;
  }
};

#endif
