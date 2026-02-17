#ifndef SENSOR_MS2_H
#define SENSOR_MS2_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"

// ========== MEGASQUIRT 2 PROTOCOL ==========

// MegaSquirt 2 usa protocolo serial binario
// Comando 'A' request realtime data block
// Respuesta: 212 bytes de datos (MS2 Extra firmware 3.4.4)

// Engine status flags (MS2 engine byte - offset 11)
enum EngineStatusFlag {
  READY    = 0,  // bit 0
  CRANK    = 1,  // bit 1
  ASE      = 2,  // bit 2
  WARMUP   = 3,  // bit 3
  TPS_AE   = 4,  // bit 4
  LAUNCH   = 6,  // bit 6
  FLATSHIFT= 7   // bit 7
};

// Status2 flags (MS2 status2 byte - offset 79)
enum Status2Flag {
  CL_IDLE  = 7   // bit 7 - Closed Loop Idle active
};

// Offsets en el data block (MS2 Extra firmware 3.4.4)
#define MS2_SECONDS       0   // U16 - seconds
#define MS2_PW1_MSB       2   // U16 - Pulse Width 1 (ms * 1500)
#define MS2_PW1_LSB       3
#define MS2_RPM_MSB       6   // U16 - RPM
#define MS2_RPM_LSB       7
#define MS2_ADV_MSB       8   // S16 - Spark Advance (deg * 10)
#define MS2_ADV_LSB       9
#define MS2_ENGINE        11  // U08 - Engine status flags
#define MS2_AFR_TARGET1   12  // U08 - AFR Target (afr * 10)
#define MS2_AFR1          28  // S16 - AFR actual (afr * 10)
#define MS2_AFR1_LSB      29
#define MS2_BARO_MSB      16  // S16 - Barometric (kPa * 10)
#define MS2_BARO_LSB      17
#define MS2_MAP_MSB       18  // S16 - MAP (kPa * 10)
#define MS2_MAP_LSB       19
#define MS2_MAT_MSB       20  // S16 - Manifold Air Temp (°F * 10)
#define MS2_MAT_LSB       21
#define MS2_CLT_MSB       22  // S16 - Coolant Temp (°F * 10)
#define MS2_CLT_LSB       23
#define MS2_TPS_MSB       24  // S16 - TPS (% * 10)
#define MS2_TPS_LSB       25
#define MS2_BAT_MSB       26  // S16 - Battery Voltage (V * 10)
#define MS2_BAT_LSB       27
#define MS2_IACSTEP_MSB   54  // S16 - IAC step / Idle PWM (duty = value * 0.392)
#define MS2_IACSTEP_LSB   55
#define MS2_DWELL_MSB     62  // U16 - Dwell (ms * 15)
#define MS2_DWELL_LSB     63
#define MS2_STATUS2       79  // U08 - Status2 flags
#define MS2_ADC6_MSB      128 // U16 - ADC6 (Fuel Pressure sensor)
#define MS2_ADC6_LSB      129
#define MS2_ADC7_MSB      130 // U16 - ADC7 (Oil Pressure sensor)
#define MS2_ADC7_LSB      131

#define MS2_DATA_SIZE     212  // Tamaño del bloque de datos

// ========== SENSOR MS2 CLASS ==========

class SensorMS2 {
private:
  uint8_t dataBlock[MS2_DATA_SIZE];
  unsigned long lastRequest;
  unsigned long lastResponse;
  bool dataValid;

  // Encontrar configuración de un valor
  const ValueConfig* getConfig(ValueType type) {
    static ValueConfig tempCfg;
    for (uint8_t i = 0; i < VALUE_CONFIG_COUNT; i++) {
      memcpy_P(&tempCfg, &VALUE_CONFIGS[i], sizeof(ValueConfig));
      if (tempCfg.type == type && tempCfg.source == SOURCE_MS2) {
        return &tempCfg;
      }
    }
    return nullptr;
  }

  // Leer word (2 bytes) del data block
  uint16_t readWord(uint8_t offset) {
    return (dataBlock[offset] << 8) | dataBlock[offset + 1];
  }

  // Leer word signed (2 bytes) del data block
  int16_t readWordSigned(uint8_t offset) {
    return (int16_t)((dataBlock[offset] << 8) | dataBlock[offset + 1]);
  }

  // Parsear valor desde data block según tipo
  float parseValue(ValueType type) {
    switch (type) {
      case VALUE_MAP: {
        // MAP en kPa * 10 desde MS2, devolver en kPa
        int16_t kpa10 = readWordSigned(MS2_MAP_MSB);
        return kpa10 / 10.0; // Devolver en kPa directamente
      }
      
      case VALUE_AIR_TEMP: {
        // MAT en °F * 10, convertir a °C
        int16_t tempF10 = readWordSigned(MS2_MAT_MSB);
        float tempF = tempF10 / 10.0;
        return (tempF - 32.0) * 5.0 / 9.0;
      }
      
      case VALUE_COOLANT_TEMP: {
        // CLT en °F * 10, convertir a °C
        int16_t tempF10 = readWordSigned(MS2_CLT_MSB);
        float tempF = tempF10 / 10.0;
        return (tempF - 32.0) * 5.0 / 9.0;
      }
      
      case VALUE_RPM: {
        // RPM directo
        return readWord(MS2_RPM_MSB);
      }
      
      case VALUE_TPS: {
        // TPS en % * 10
        int16_t tps10 = readWordSigned(MS2_TPS_MSB);
        return tps10 / 10.0;
      }
      
      case VALUE_BATTERY: {
        // Battery en V * 10
        int16_t batt10 = readWordSigned(MS2_BAT_MSB);
        return batt10 / 10.0;
      }
      
      case VALUE_AFR: {
        // AFR * 10 (S16)
        int16_t afr10 = readWordSigned(MS2_AFR1);
        return afr10 / 10.0;
      }
      
      case VALUE_IGNITION: {
        // Advance en grados * 10
        int16_t adv10 = readWordSigned(MS2_ADV_MSB);
        return adv10 / 10.0;
      }
      
      case VALUE_DWELL: {
        // Dwell en ms * 15 (0.0666 scale)
        uint16_t dwell15 = readWord(MS2_DWELL_MSB);
        return dwell15 * 0.0666;
      }
      
      case VALUE_FUEL_PRESSURE: {
        // Fuel pressure desde ADC6 (raw ADC 0-1023)
        // Convertir a PSI usando fórmula del custom.ini:
        // PSI = 0 + (150 - 0) * ((adc6 - 102.3) / (920.7 - 102.3))
        uint16_t adc6 = readWord(MS2_ADC6_MSB);
        float psi = 150.0 * ((float)adc6 - 102.3) / (920.7 - 102.3);
        return psi < 0 ? 0 : psi;
      }
      
      case VALUE_OIL_PRESSURE: {
        // Oil pressure desde ADC7 (raw ADC 0-1023)
        // Convertir a PSI usando fórmula del custom.ini:
        // PSI = 0 + (150 - 0) * ((adc7 - 102.3) / (920.7 - 102.3))
        uint16_t adc7 = readWord(MS2_ADC7_MSB);
        float psi = 150.0 * ((float)adc7 - 102.3) / (920.7 - 102.3);
        return psi < 0 ? 0 : psi;
      }
      
      case VALUE_PULSE_WIDTH: {
        // Pulse Width en 0.000666 scale
        uint16_t pw = readWord(MS2_PW1_MSB);
        return pw * 0.000666; // Convertir a ms
      }
      
      case VALUE_IDLE_PWM: {
        // Idle PWM duty cycle (iacstep * 0.392)
        int16_t iacstep = readWordSigned(MS2_IACSTEP_MSB);
        return iacstep * 0.392;
      }
      
      // Engine status flags individuales (MS2 engine byte - offset 11)
      case VALUE_ENGINE_READY:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::READY))) ? 1.0 : 0.0;
      case VALUE_ENGINE_CRANK:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::CRANK))) ? 1.0 : 0.0;
      case VALUE_ENGINE_ASE:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::ASE))) ? 1.0 : 0.0;
      case VALUE_ENGINE_WARMUP:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::WARMUP))) ? 1.0 : 0.0;
      case VALUE_ENGINE_TPS_AE:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::TPS_AE))) ? 1.0 : 0.0;
      case VALUE_ENGINE_LAUNCH:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::LAUNCH))) ? 1.0 : 0.0;
      case VALUE_ENGINE_FLATSHIFT:
        return (dataBlock[MS2_ENGINE] & (1 << static_cast<int>(EngineStatusFlag::FLATSHIFT))) ? 1.0 : 0.0;
      
      // Status2 flags (MS2 status2 byte - offset 79)
      case VALUE_CL_IDLE:
        return (dataBlock[MS2_STATUS2] & (1 << static_cast<int>(Status2Flag::CL_IDLE))) ? 1.0 : 0.0;
      
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
