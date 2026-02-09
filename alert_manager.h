#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include <Arduino.h>
#include "data_types.h"
#include "config.h"
#include "data_manager.h"

// ========== ALERT MANAGER CLASS ==========

// Mensajes de alerta en PROGMEM centrados con asteriscos (16 chars cada línea)
const char alert_cold_load[] PROGMEM = "***MOTOR FRIO***";
const char alert_cold_load2[] PROGMEM = "**NO ACELERAR!**";
const char alert_low_oil[] PROGMEM = "**BAJA PRESION**";
const char alert_low_oil2[] PROGMEM = "*****ACEITE*****";
const char alert_high_temp[] PROGMEM = "**SOBRECALENT.**";
const char alert_high_temp2[] PROGMEM = "*DETENER MOTOR!*";
const char alert_oil_press[] PROGMEM = "**OIL PRESSURE**";
const char alert_low[] PROGMEM = "******LOW!******";
const char alert_oil_temp[] PROGMEM = "*OIL TEMP HIGH!*";
const char alert_coolant[] PROGMEM = "**COOLANT HIGH**";
const char alert_battery[] PROGMEM = "**BATTERY VOLT**";
const char alert_afr[] PROGMEM = "***AFR UNSAFE***";

class AlertManager {
private:
  DataManager* dataManager;
  bool alertActive;
  AlertSeverity currentSeverity;
  unsigned long lastBlinkTime;
  bool blinkState;
  unsigned long alertStartTime;  // Cuándo se activó la alerta
  bool wasAlertActive;           // Estado previo para detectar nueva alerta
  
  // Tracking de alerta activa
  const char* alertLine1;
  const char* alertLine2;

  // Verificar una alerta simple (rango)
  bool checkRangeAlert(const AlertRange& alert) {
    if (!alert.enabled) return false;
    
    float value = dataManager->getValue(alert.type);
    return (value < alert.min || value > alert.max);
  }

  // Alerta: Motor frío + carga alta
  bool checkColdEngineHighLoad() {
    float clt = dataManager->getValue(VALUE_COOLANT_TEMP);
    float olt = dataManager->getValue(VALUE_OIL_TEMP);
    float tps = dataManager->getValue(VALUE_TPS);
    float map = dataManager->getValue(VALUE_MAP);
    
    // Motor considerado frío si ambos < 85°C
    bool engineCold = (clt < 85.0 && olt < 85.0);
    // Alta carga si TPS > 50% o MAP > 5 PSI boost
    bool highLoad = (tps > 50.0 || map > 5.0);
    
    return engineCold && highLoad;
  }

  // Alerta: Baja presión aceite con RPM altas
  bool checkLowOilPressure() {
    float oilPress = dataManager->getValue(VALUE_OIL_PRESSURE);
    float rpm = dataManager->getValue(VALUE_RPM);
    
    // Presión crítica: < 10 PSI con > 2000 RPM
    return (oilPress < 10.0 && rpm > 2000.0);
  }

  // Alerta: Temperaturas combinadas altas
  bool checkHighTempCombo() {
    float clt = dataManager->getValue(VALUE_COOLANT_TEMP);
    float olt = dataManager->getValue(VALUE_OIL_TEMP);
    
    // Ambas temperaturas altas
    return (clt > 105.0 && olt > 120.0);
  }

public:
  AlertManager(DataManager* dm) {
    dataManager = dm;
    alertActive = false;
    currentSeverity = ALERT_WARNING;
    lastBlinkTime = 0;
    blinkState = false;
    alertStartTime = 0;
    wasAlertActive = false;
    alertLine1 = nullptr;
    alertLine2 = nullptr;
  }

  // Verificar todas las alertas
  void update() {
    bool previouslyActive = alertActive;
    alertActive = false;
    currentSeverity = ALERT_WARNING;
    alertLine1 = nullptr;
    alertLine2 = nullptr;

    // Verificar alertas simples (rango)
    for (uint8_t i = 0; i < ALERT_RANGE_COUNT; i++) {
      AlertRange alert;
      memcpy_P(&alert, &ALERT_RANGES[i], sizeof(AlertRange));
      
      if (checkRangeAlert(alert)) {
        alertActive = true;
        if (alert.severity == ALERT_CRITICAL) {
          currentSeverity = ALERT_CRITICAL;
        }
        
        // Asignar mensaje según tipo (prioridad a críticos)
        if (alert.type == VALUE_OIL_PRESSURE && alert.severity == ALERT_CRITICAL) {
          alertLine1 = alert_oil_press;
          alertLine2 = alert_low;
        }
        else if (alert.type == VALUE_OIL_TEMP) {
          alertLine1 = alert_oil_temp;
          alertLine2 = nullptr;
        }
        else if (alert.type == VALUE_COOLANT_TEMP) {
          alertLine1 = alert_coolant;
          alertLine2 = nullptr;
        }
        else if (alert.type == VALUE_BATTERY) {
          alertLine1 = alert_battery;
          alertLine2 = nullptr;
        }
        else if (alert.type == VALUE_AFR) {
          alertLine1 = alert_afr;
          alertLine2 = nullptr;
        }
      }
    }

    // Verificar alertas compuestas (prioridad sobre simples)
    if (checkColdEngineHighLoad()) {
      alertActive = true;
      currentSeverity = ALERT_WARNING;
      alertLine1 = alert_cold_load;
      alertLine2 = alert_cold_load2;
    }

    if (checkLowOilPressure()) {
      alertActive = true;
      currentSeverity = ALERT_CRITICAL;
      alertLine1 = alert_low_oil;
      alertLine2 = alert_low_oil2;
    }

    if (checkHighTempCombo()) {
      alertActive = true;
      currentSeverity = ALERT_CRITICAL;
      alertLine1 = alert_high_temp;
      alertLine2 = alert_high_temp2;
    }
    
    // Detectar nueva alerta para iniciar timer de blink
    if (alertActive && !previouslyActive) {
      alertStartTime = millis();
    }
    
    wasAlertActive = alertActive;
  }

  // Verificar si hay alguna alerta activa
  bool isAlertActive() {
    return alertActive;
  }

  // Obtener severidad de alerta actual
  AlertSeverity getSeverity() {
    return currentSeverity;
  }

  // Actualizar estado de blink (solo titila los primeros segundos)
  bool shouldBlink() {
    if (!alertActive) return true;  // Sin alerta, luz siempre encendida
    
    // Durá titilar solo 3 segundos para llamar atención
    unsigned long elapsedTime = millis() - alertStartTime;
    const unsigned long BLINK_DURATION = 3000;  // 3 segundos de blink
    
    if (elapsedTime > BLINK_DURATION) {
      return true;  // Después de 3s, luz fija para leer
    }
    
    // Durante los primeros 3s, titilar según severidad
    unsigned long blinkInterval = (currentSeverity == ALERT_CRITICAL) ? 200 : 500;
    
    if (millis() - lastBlinkTime >= blinkInterval) {
      blinkState = !blinkState;
      lastBlinkTime = millis();
    }

    return blinkState;
  }

  // Reset estado de alerta
  void reset() {
    alertActive = false;
    blinkState = false;
    alertStartTime = 0;
    wasAlertActive = false;
    alertLine1 = nullptr;
    alertLine2 = nullptr;
  }
  
  // Obtener mensajes de alerta
  const char* getAlertLine1() {
    return alertLine1;
  }
  
  const char* getAlertLine2() {
    return alertLine2;
  }
};

#endif
