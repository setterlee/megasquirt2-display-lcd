#ifndef SENSOR_DIRECT_H
#define SENSOR_DIRECT_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"

// ========== SENSOR DIRECT CLASS ==========

class SensorDirect {
private:
  // Encontrar configuración de un valor
  const ValueConfig* getConfig(ValueType type) {
    static ValueConfig tempCfg;
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      memcpy_P(&tempCfg, &VALUE_CONFIGS[i], sizeof(ValueConfig));
      if (tempCfg.type == type && tempCfg.source == SOURCE_DIRECT) {
        return &tempCfg;
      }
    }
    return nullptr;
  }

  // Mapear valor raw a valor real con calibración
  float mapValue(int raw, const ValueConfig* config) {
    if (!config) return 0.0;
    
    // Clamp raw value dentro del rango
    raw = constrain(raw, config->minRaw, config->maxRaw);
    
    // Map linealmente
    return map(raw, config->minRaw, config->maxRaw, 
               (int)(config->minReal * 100), 
               (int)(config->maxReal * 100)) / 100.0;
  }

public:
  // Inicializar pines (llamar en setup)
  void begin() {
    // Los pines analógicos no necesitan pinMode en Arduino
    // pero podemos hacer una verificación si queremos
  }

  // Leer un valor desde sensor directo
  float read(ValueType type) {
    const ValueConfig* config = getConfig(type);
    if (!config) return 0.0;

    int raw = analogRead(config->pin);
    return mapValue(raw, config);
  }

  // Leer raw sin calibración (útil para debug)
  int readRaw(ValueType type) {
    const ValueConfig* config = getConfig(type);
    if (!config) return 0;
    return analogRead(config->pin);
  }

  // Verificar si un tipo de valor es de sensor directo
  bool isDirect(ValueType type) {
    const ValueConfig* config = getConfig(type);
    return config != nullptr;
  }
};

#endif
