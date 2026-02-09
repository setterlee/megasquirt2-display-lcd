#ifndef SENSOR_MS2_H
#define SENSOR_MS2_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"

// ========== MEGASQUIRT 2 PROTOCOL ==========

// MegaSquirt 2 usa protocolo serial binario
// Comando 'A' request realtime data block
// Respuesta: 75 bytes de datos

// Offsets en el data block (MS2 Extra firmware)
#define MS2_SECONDS       0   // 1 byte
#define MS2_PW1_MSB       1   // 2 bytes - Pulse Width 1
#define MS2_PW1_LSB       2
#define MS2_RPM_MSB       6   // 2 bytes - RPM
#define MS2_RPM_LSB       7
#define MS2_ADV_MSB       8   // 2 bytes - Spark Advance
#define MS2_ADV_LSB       9
#define MS2_ENGINE        10  // 1 byte - Engine status
#define MS2_AFR_TARGET1   11  // 1 byte - AFR Target
#define MS2_AFR1          12  // 1 byte - AFR actual
#define MS2_BARO_MSB      13  // 2 bytes - Barometric
#define MS2_BARO_LSB      14
#define MS2_MAP_MSB       15  // 2 bytes - MAP
#define MS2_MAP_LSB       16
#define MS2_MAT_MSB       17  // 2 bytes - Manifold Air Temp
#define MS2_MAT_LSB       18
#define MS2_CLT_MSB       19  // 2 bytes - Coolant Temp
#define MS2_CLT_LSB       20
#define MS2_TPS_MSB       21  // 2 bytes - TPS
#define MS2_TPS_LSB       22
#define MS2_BAT_MSB       23  // 2 bytes - Battery Voltage
#define MS2_BAT_LSB       24
#define MS2_DWELL_MSB     25  // 2 bytes - Dwell
#define MS2_DWELL_LSB     26

#define MS2_DATA_SIZE     75  // Tamaño del bloque de datos

// ========== SENSOR MS2 CLASS ==========

class SensorMS2 {
private:
  uint8_t dataBlock[MS2_DATA_SIZE];
  unsigned long lastRequest;
  unsigned long lastResponse;
  bool dataValid;

  // Encontrar configuración de un valor
  const ValueConfig* getConfig(ValueType type) {
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      if (VALUE_CONFIGS[i].type == type && VALUE_CONFIGS[i].source == SOURCE_MS2) {
        return &VALUE_CONFIGS[i];
      }
    }
    return nullptr;
  }

  // Leer word (2 bytes) del data block
  uint16_t readWord(uint8_t offset) {
    return (dataBlock[offset] << 8) | dataBlock[offset + 1];
  }

  // Parsear valor desde data block según tipo
  float parseValue(ValueType type) {
    switch (type) {
      case VALUE_MAP: {
        // MAP en kPa * 10, convertir a PSI
        uint16_t kpa10 = readWord(MS2_MAP_MSB);
        float kpa = kpa10 / 10.0;
        float bar = kpa / 100.0;
        return bar * 14.5038; // Convertir a PSI
      }
      
      case VALUE_AIR_TEMP: {
        // MAT en °F * 10, convertir a °C
        uint16_t tempF10 = readWord(MS2_MAT_MSB);
        float tempF = tempF10 / 10.0;
        return (tempF - 32.0) * 5.0 / 9.0;
      }
      
      case VALUE_COOLANT_TEMP: {
        // CLT en °F * 10, convertir a °C
        uint16_t tempF10 = readWord(MS2_CLT_MSB);
        float tempF = tempF10 / 10.0;
        return (tempF - 32.0) * 5.0 / 9.0;
      }
      
      case VALUE_RPM: {
        // RPM directo
        return readWord(MS2_RPM_MSB);
      }
      
      case VALUE_TPS: {
        // TPS en % * 10
        uint16_t tps10 = readWord(MS2_TPS_MSB);
        return tps10 / 10.0;
      }
      
      case VALUE_AFR: {
        // AFR * 10
        uint8_t afr10 = dataBlock[MS2_AFR1];
        return afr10 / 10.0;
      }
      
      case VALUE_IGNITION: {
        // Advance en grados * 10
        uint16_t adv10 = readWord(MS2_ADV_MSB);
        return adv10 / 10.0;
      }
      
      case VALUE_DWELL: {
        // Dwell en ms * 10
        uint16_t dwell10 = readWord(MS2_DWELL_MSB);
        return dwell10 / 10.0;
      }
      
      default:
        return 0.0;
    }
  }

public:
  // Inicializar comunicación serial
  void begin() {
    MS2_SERIAL.begin(MS2_BAUD_RATE);
    lastRequest = 0;
    lastResponse = 0;
    dataValid = false;
    memset(dataBlock, 0, MS2_DATA_SIZE);
  }

  // Actualizar - llamar en loop
  void update() {
    // Solicitar datos cada UPDATE_INTERVAL
    if (millis() - lastRequest >= UPDATE_INTERVAL) {
      requestData();
      lastRequest = millis();
    }

    // Leer respuesta si hay datos disponibles
    if (MS2_SERIAL.available() >= MS2_DATA_SIZE) {
      readData();
    }

    // Validar timeout
    if (millis() - lastResponse > MS2_TIMEOUT) {
      dataValid = false;
    }
  }

  // Solicitar datos a MS2
  void requestData() {
    MS2_SERIAL.write('A'); // Comando 'A' = request realtime data
  }

  // Leer datos de MS2
  void readData() {
    uint8_t bytesRead = 0;
    unsigned long start = millis();
    
    while (bytesRead < MS2_DATA_SIZE && millis() - start < 100) {
      if (MS2_SERIAL.available()) {
        dataBlock[bytesRead++] = MS2_SERIAL.read();
      }
    }

    if (bytesRead == MS2_DATA_SIZE) {
      dataValid = true;
      lastResponse = millis();
    }
  }

  // Leer un valor desde MS2
  float read(ValueType type) {
    if (!dataValid) return 0.0;
    return parseValue(type);
  }

  // Verificar si hay datos válidos
  bool isValid() {
    return dataValid;
  }

  // Verificar si un tipo de valor viene de MS2
  bool isMS2(ValueType type) {
    const ValueConfig* config = getConfig(type);
    return config != nullptr;
  }

  // Limpiar buffer serial (útil para debug)
  void flush() {
    while (MS2_SERIAL.available()) {
      MS2_SERIAL.read();
    }
  }
};

#endif
