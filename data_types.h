#ifndef DATA_TYPES_H
#define DATA_TYPES_H

// ========== ENUMS ==========

// Tipos de valores disponibles
enum ValueType {
  VALUE_MAP,           // Manifold Absolute Pressure (boost)
  VALUE_OIL_PRESSURE,  // Presión de aceite
  VALUE_OIL_TEMP,      // Temperatura de aceite
  VALUE_AIR_TEMP,      // Temperatura de aire (IAT)
  VALUE_BATTERY,       // Voltaje de batería
  VALUE_RPM,           // Revoluciones por minuto (MS2)
  VALUE_TPS,           // Throttle Position Sensor (MS2)
  VALUE_COOLANT_TEMP,  // Temperatura de refrigerante (MS2)
  VALUE_AFR,           // Air/Fuel Ratio (MS2)
  VALUE_IGNITION,      // Avance de ignición (MS2)
  VALUE_DWELL,         // Dwell time (MS2)
  VALUE_FUEL_PRESSURE,   // Presión de combustible (MS2)
  VALUE_PULSE_WIDTH,     // Pulse Width 1 (MS2)
  VALUE_ENGINE_READY,    // Engine Ready flag (MS2)
  VALUE_ENGINE_CRANK,    // Cranking flag (MS2)
  VALUE_ENGINE_ASE,      // After Start Enrichment flag (MS2)
  VALUE_ENGINE_WARMUP,   // Warmup Enrichment flag (MS2)
  VALUE_ENGINE_TPS_AE,   // TPS Acceleration Enrichment flag (MS2)
  VALUE_ENGINE_LAUNCH,   // Launch Control flag (MS2)
  VALUE_ENGINE_FLATSHIFT,// Flat Shift flag (MS2)
  VALUE_IDLE_PWM,        // Idle PWM (MS2)
  VALUE_CL_IDLE,         // Closed Loop Idle flag (MS2)
  VALUE_NONE             // Sin valor
};

// Severidad de alerta
enum AlertSeverity {
  ALERT_WARNING,       // Advertencia (amarillo/titilado lento)
  ALERT_CRITICAL       // Crítico (rojo/titilado rápido)
};

// Tipos de alertas compuestas
enum CompoundAlertType {
  ALERT_COLD_ENGINE_HIGH_LOAD,  // Motor frío + carga alta
  ALERT_LOW_OIL_PRESSURE,       // Baja presión aceite + RPM altas
  ALERT_HIGH_TEMP_COMBO,        // Temp aceite + coolant altas
  COMPOUND_ALERT_COUNT
};

// Fuentes de datos
enum DataSource {
  SOURCE_DIRECT,       // Sensor conectado directamente (pin analógico)
  SOURCE_MS2,          // Dato desde MegaSquirt 2 (serial)
  SOURCE_TEST          // Modo test (valores simulados)
};

// Unidades de medida
enum Unit {
  UNIT_BAR,            // Bar (presión)
  UNIT_PSI,            // PSI (presión)
  UNIT_KPA,            // kPa (presión)
  UNIT_CELSIUS,        // Celsius (temperatura)
  UNIT_FAHRENHEIT,     // Fahrenheit (temperatura)
  UNIT_VOLT,           // Voltios
  UNIT_RPM,            // Revoluciones por minuto
  UNIT_PERCENT,        // Porcentaje
  UNIT_DEGREES,        // Grados (ángulo)
  UNIT_MS,             // Milisegundos
  UNIT_RATIO           // Ratio (AFR)
};

// ========== ESTRUCTURAS ==========

// Configuración de un valor individual
struct ValueConfig {
  ValueType type;      // Tipo de valor
  DataSource source;   // Fuente de datos
  const char* label;   // Etiqueta corta en PROGMEM (3-4 chars para LCD)
  Unit unit;           // Unidad de medida
  uint8_t pin;         // Pin analógico (si SOURCE_DIRECT)
  int minRaw;          // Valor mínimo raw del ADC/MS2
  int maxRaw;          // Valor máximo raw del ADC/MS2
  float minReal;       // Valor mínimo real (después de calibración)
  float maxReal;       // Valor máximo real (después de calibración)
  uint8_t decimals;    // Decimales a mostrar
};

// Elemento de display (valor + unidad)
struct DisplayItem {
  ValueType value;     // Qué valor mostrar
  bool showUnit;       // Mostrar unidad?
  bool showSign;       // Mostrar signo + para positivos?
};

// Rango de alerta para un sensor
struct AlertRange {
  ValueType type;      // Tipo de valor a monitorear
  float min;           // Valor mínimo seguro
  float max;           // Valor máximo seguro
  AlertSeverity severity; // Severidad de la alerta
  bool enabled;        // Activar alerta?
};

// Configuración de una línea del LCD (2 valores)
struct LineConfig {
  DisplayItem item1;
  DisplayItem item2;
};

// Configuración de una página completa
struct PageConfig {
  LineConfig line1;    // Primera línea (16 caracteres)
  LineConfig line2;    // Segunda línea (16 caracteres)
};

// Datos en tiempo real de un valor
struct ValueData {
  float current;       // Valor actual
  float min;           // Mínimo registrado
  float max;           // Máximo registrado
  unsigned long lastUpdate; // Timestamp última actualización
  bool valid;          // Dato válido?
};

#endif
