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
const char alert_boost_low[] PROGMEM = "Pres. Boost Baja!";
const char alert_boost_low2[] PROGMEM = "Nooo..! Monica!!!";
const char alert_boost_high[] PROGMEM = "Pres. Boost Alta!";
const char alert_boost_high2[] PROGMEM = "Danger 2 Manifold";

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
  
  // Boost monitoring state variables
  bool underboost_active;
  bool overboost_active;
  bool overboost_critical;
  unsigned long underboost_condition_start;
  unsigned long overboost_condition_start;
  bool underboost_condition_met;
  bool overboost_condition_met;

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
  
  // 🎯 Evaluar alertas de boost con condiciones sofisticadas
  // Retorna: 0 = OK, -1 = underboost, 1 = overboost, 2 = overboost crítico
  int checkBoostDeviation() {
    float mapActual = dataManager->getValue(VALUE_MAP);
    float mapTarget = dataManager->getValue(VALUE_MAP_TARGET);
    float tps = dataManager->getValue(VALUE_TPS);
    float rpm = dataManager->getValue(VALUE_RPM);
    
    // 🎯 CONDICIONES GENERALES - Verificar si debemos evaluar boost
    bool shouldEvaluate = (tps > 70.0) && (rpm > 3000.0) && (mapTarget > 150.0);
    
    if (!shouldEvaluate) {
      // Resetear todo si no cumplimos condiciones
      underboost_active = false;
      overboost_active = false;
      overboost_critical = false;
      underboost_condition_met = false;
      overboost_condition_met = false;
      underboost_condition_start = 0;
      overboost_condition_start = 0;
      return 0;
    }
    
    // Calcular desviación
    float deviation = mapActual - mapTarget;
    unsigned long now = millis();
    
    // 🔥 OVERBOOST CRÍTICO - Activación inmediata
    if (deviation > 25.0) {
      overboost_critical = true;
      return 2;  // Crítico
    }
    
    // Si el crítico estaba activo pero ya no, desactivarlo con histéresis
    if (overboost_critical && deviation <= 5.0) {
      overboost_critical = false;
    }
    
    // Si crítico sigue activo, retornarlo
    if (overboost_critical) {
      return 2;
    }
    
    // 🚨 OVERBOOST - Activar con +12 kPa durante 400ms
    if (deviation > 12.0) {
      if (!overboost_condition_met) {
        // Primera vez que detectamos la condición
        overboost_condition_met = true;
        overboost_condition_start = now;
      } else {
        // Verificar si ha pasado el tiempo necesario
        if (now - overboost_condition_start >= 400) {
          overboost_active = true;
        }
      }
    } else {
      // Condición no se cumple, resetear
      overboost_condition_met = false;
      overboost_condition_start = 0;
    }
    
    // Histéresis para desactivar overboost (volver a +5 kPa del target)
    if (overboost_active && deviation <= 5.0) {
      overboost_active = false;
    }
    
    // 🚨 UNDERBOOST - Activar con -15 kPa durante 800ms
    if (deviation < -15.0) {
      if (!underboost_condition_met) {
        // Primera vez que detectamos la condición
        underboost_condition_met = true;
        underboost_condition_start = now;
      } else {
        // Verificar si ha pasado el tiempo necesario
        if (now - underboost_condition_start >= 800) {
          underboost_active = true;
        }
      }
    } else {
      // Condición no se cumple, resetear
      underboost_condition_met = false;
      underboost_condition_start = 0;
    }
    
    // Histéresis para desactivar underboost (volver a -5 kPa del target)
    if (underboost_active && deviation >= -5.0) {
      underboost_active = false;
    }
    
    // Retornar estado actual
    if (overboost_active) {
      return 1;  // Overboost
    } else if (underboost_active) {
      return -1;  // Underboost
    }
    
    return 0;  // OK
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
    
    // Inicializar boost monitoring
    underboost_active = false;
    overboost_active = false;
    overboost_critical = false;
    underboost_condition_start = 0;
    overboost_condition_start = 0;
    underboost_condition_met = false;
    overboost_condition_met = false;
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
    
    // Verificar desviación de boost (alerta especial)
    int boostStatus = checkBoostDeviation();
    if (boostStatus == -1) {  // Underboost
      alertActive = true;
      currentSeverity = ALERT_WARNING;
      alertLine1 = alert_boost_low;
      alertLine2 = alert_boost_low2;
    } else if (boostStatus == 1) {  // Overboost
      alertActive = true;
      currentSeverity = ALERT_CRITICAL;
      alertLine1 = alert_boost_high;
      alertLine2 = alert_boost_high2;
    } else if (boostStatus == 2) {  // Overboost crítico
      alertActive = true;
      currentSeverity = ALERT_CRITICAL;
      alertLine1 = alert_boost_high;
      alertLine2 = alert_boost_high2;
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
    
    // Resetear boost monitoring
    underboost_active = false;
    overboost_active = false;
    overboost_critical = false;
    underboost_condition_start = 0;
    overboost_condition_start = 0;
    underboost_condition_met = false;
    overboost_condition_met = false;
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
