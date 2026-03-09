#ifndef SENSOR_DIRECT_H
#define SENSOR_DIRECT_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"

// ========== NTC LOOKUP TABLE ==========
// Tabla de conversión ADC → Temperatura para sensor NTC de aceite
// Hardware: Divisor de voltaje con R_fija = 10KΩ
// Circuito: 5V --[10K]-- Pin A2 --[NTC]-- GND

struct NTCPoint {
  int adc;      // Valor ADC (0-1023)
  float temp;   // Temperatura en °C
};

// Tabla en PROGMEM (ahorra RAM)
const NTCPoint OIL_TEMP_TABLE[] PROGMEM = {
  {881, 20.0},   // 62.32K
  {819, 30.0},   // 40.34K
  {743, 40.0},   // 26.65K
  {656, 50.0},   // 17.95K
  {564, 60.0},   // 12.32K
  {473, 70.0},   // 8.61K
  {389, 80.0},   // 6.14K
  {313, 90.0},   // 4.43K
  {255, 100.0},  // 3.32K
  {207, 110.0},  // 2.55K
  {168, 120.0},  // 1.97K
  {134, 130.0},  // 1.51K
  {107, 140.0},  // 1.17K
  {85,  150.0}   // 0.90K
};

const uint8_t OIL_TEMP_TABLE_SIZE = sizeof(OIL_TEMP_TABLE) / sizeof(OIL_TEMP_TABLE[0]);

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

  // Interpolación lineal para lookup table NTC
  float interpolateNTC(int raw) {
    // Clamp a rango válido
    if (raw >= pgm_read_word(&OIL_TEMP_TABLE[0].adc)) {
      return pgm_read_float(&OIL_TEMP_TABLE[0].temp);
    }
    if (raw <= pgm_read_word(&OIL_TEMP_TABLE[OIL_TEMP_TABLE_SIZE - 1].adc)) {
      return pgm_read_float(&OIL_TEMP_TABLE[OIL_TEMP_TABLE_SIZE - 1].temp);
    }
    
    // Buscar puntos para interpolar
    for (uint8_t i = 0; i < OIL_TEMP_TABLE_SIZE - 1; i++) {
      int adc1 = pgm_read_word(&OIL_TEMP_TABLE[i].adc);
      int adc2 = pgm_read_word(&OIL_TEMP_TABLE[i + 1].adc);
      
      if (raw <= adc1 && raw >= adc2) {
        float temp1 = pgm_read_float(&OIL_TEMP_TABLE[i].temp);
        float temp2 = pgm_read_float(&OIL_TEMP_TABLE[i + 1].temp);
        
        // Interpolación lineal entre puntos
        float ratio = (float)(raw - adc2) / (float)(adc1 - adc2);
        return temp2 + ratio * (temp1 - temp2);
      }
    }
    
    return 0.0; // Fallback
  }

  // Mapear valor raw a valor real con calibración lineal
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
    
    // Usar lookup table para sensor NTC de temperatura de aceite
    if (type == VALUE_OIL_TEMP) {
      return interpolateNTC(raw);
    }
    
    // Mapeo lineal para otros sensores
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
