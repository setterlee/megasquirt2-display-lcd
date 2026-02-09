#ifndef CONFIG_H
#define CONFIG_H

#include "data_types.h"

// ========== HARDWARE CONFIG ==========

// Pines de botones
#define BTN_PAGE 2
#define BTN_PEAK 3

// Pines I2C (fijos en Arduino UNO)
// SDA: A4
// SCL: A5

// Dirección LCD I2C
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Puerto serial para MS2
#define MS2_SERIAL Serial
#define MS2_BAUD_RATE 9600

// ========== TIMING CONFIG ==========

const unsigned long UPDATE_INTERVAL   = 250;   // ms entre actualizaciones
const unsigned long TEST_INTERVAL     = 120;   // ms entre pasos de test
const unsigned long PEAK_VIEW_TIME    = 3000;  // ms mostrando MIN/MAX
const unsigned long LONG_PRESS_TIME   = 1500;  // ms para detectar long press
const unsigned long UNIT_MSG_TIME     = 3000;  // ms mostrando cambio de unidad
const unsigned long MS2_TIMEOUT       = 1000;  // ms timeout para MS2

// ========== VALUE DEFINITIONS ==========

// Labels en PROGMEM (ahorra RAM)
const char label_MAP[] PROGMEM = "MAP";
const char label_OIL[] PROGMEM = "OIL";
const char label_OLT[] PROGMEM = "OLT";
const char label_IAT[] PROGMEM = "IAT";
const char label_BAT[] PROGMEM = "BAT";
const char label_RPM[] PROGMEM = "RPM";
const char label_TPS[] PROGMEM = "TPS";
const char label_CLT[] PROGMEM = "CLT";
const char label_AFR[] PROGMEM = "AFR";
const char label_IGN[] PROGMEM = "IGN";
const char label_FUP[] PROGMEM = "FUP";
const char label_PW1[] PROGMEM = "PW1";
const char label_RDY[] PROGMEM = "RDY";
const char label_CRK[] PROGMEM = "CRK";
const char label_ASE[] PROGMEM = "ASE";
const char label_WUE[] PROGMEM = "WUE";
const char label_TPS2[] PROGMEM = "TPS";
const char label_LCH[] PROGMEM = "LCH";
const char label_FSH[] PROGMEM = "FSH";

// Configuración de cada valor disponible
const ValueConfig VALUE_CONFIGS[] PROGMEM PROGMEM = {
  // type              source          label      unit          pin  minRaw maxRaw  minReal maxReal decimals
  {VALUE_MAP,          SOURCE_MS2,     label_MAP, UNIT_PSI,     0,   0,     1023,   -14.5,  30.0,   1},
  {VALUE_OIL_PRESSURE, SOURCE_DIRECT,  label_OIL, UNIT_PSI,     A1,  0,     1023,   0.0,    87.0,   1},
  {VALUE_OIL_TEMP,     SOURCE_DIRECT,  label_OLT, UNIT_CELSIUS, A2,  0,     1023,   20.0,   140.0,  0},
  {VALUE_AIR_TEMP,     SOURCE_MS2,     label_IAT, UNIT_CELSIUS, 0,   0,     1023,   -10.0,  80.0,   0},
  {VALUE_BATTERY,      SOURCE_DIRECT,  label_BAT, UNIT_VOLT,    A3,  0,     1023,   0.0,    20.0,   1},
  {VALUE_RPM,          SOURCE_MS2,     label_RPM, UNIT_RPM,     0,   0,     8000,   0.0,    8000.0, 0},
  {VALUE_TPS,          SOURCE_MS2,     label_TPS, UNIT_PERCENT, 0,   0,     100,    0.0,    100.0,  0},
  {VALUE_COOLANT_TEMP, SOURCE_MS2,     label_CLT, UNIT_CELSIUS, 0,   0,     1023,   20.0,   120.0,  0},
  {VALUE_AFR,          SOURCE_MS2,     label_AFR, UNIT_RATIO,   0,   0,     255,    10.0,   20.0,   1},
  {VALUE_IGNITION,     SOURCE_MS2,     label_IGN, UNIT_DEGREES, 0,   -10,   50,     -10.0,  50.0,   1},
  {VALUE_FUEL_PRESSURE,  SOURCE_MS2,   label_FUP, UNIT_PSI,     0,   0,     1023,   0.0,    100.0,  1},
  {VALUE_PULSE_WIDTH,    SOURCE_MS2,   label_PW1, UNIT_MS,      0,   0,     2550,   0.0,    25.5,   1},
  {VALUE_ENGINE_READY,   SOURCE_MS2,   label_RDY, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_CRANK,   SOURCE_MS2,   label_CRK, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_ASE,     SOURCE_MS2,   label_ASE, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_WARMUP,  SOURCE_MS2,   label_WUE, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_TPS_AE,  SOURCE_MS2,   label_TPS2,UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_LAUNCH,  SOURCE_MS2,   label_LCH, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
  {VALUE_ENGINE_FLATSHIFT,SOURCE_MS2,  label_FSH, UNIT_PERCENT, 0,   0,     1,      0.0,    1.0,    0},
};

const uint8_t VALUE_CONFIG_COUNT = sizeof(VALUE_CONFIGS) / sizeof(VALUE_CONFIGS[0]);

// ========== ALERT DEFINITIONS ==========

// Rangos de alerta para sensores individuales
const AlertRange ALERT_RANGES[] PROGMEM = {
  // type                min    max      severity         enabled
  {VALUE_OIL_PRESSURE,   10.0,  87.0,    ALERT_CRITICAL,  true},   // < 10 PSI crítico
  {VALUE_OIL_TEMP,       40.0,  125.0,   ALERT_WARNING,   true},   // > 125°C advertencia
  {VALUE_OIL_TEMP,       40.0,  135.0,   ALERT_CRITICAL,  true},   // > 135°C crítico
  {VALUE_COOLANT_TEMP,   60.0,  105.0,   ALERT_WARNING,   true},   // > 105°C advertencia
  {VALUE_COOLANT_TEMP,   60.0,  115.0,   ALERT_CRITICAL,  true},   // > 115°C crítico
  {VALUE_BATTERY,        11.5,  15.5,    ALERT_WARNING,   true},   // < 11.5V o > 15.5V
  {VALUE_AFR,            11.0,  16.0,    ALERT_WARNING,   true},   // Fuera de rango seguro
};

const uint8_t ALERT_RANGE_COUNT = sizeof(ALERT_RANGES) / sizeof(ALERT_RANGES[0]);

// Intervalos de blink para alertas
#define ALERT_BLINK_WARNING   500  // ms - titilado lento
#define ALERT_BLINK_CRITICAL  200  // ms - titilado rápido

// ========== PAGE DEFINITIONS ==========

// Configuración de páginas
const PageConfig PAGES[] PROGMEM = {
  // PAGE 0: MAIN - Boost y aceite
  {
    // Línea 1: MAP + Temp aire
    {
      {VALUE_MAP, true, true},      // MAP con unidad y signo +/-
      {VALUE_AIR_TEMP, true, false} // IAT con unidad
    },
    // Línea 2: Presión aceite + Temp aceite
    {
      {VALUE_OIL_PRESSURE, true, false}, // Presión aceite con unidad
      {VALUE_OIL_TEMP, true, false}      // Temp aceite con unidad
    }
  },
  
  // PAGE 1: BATTERY - Batería y modo
  {
    // Línea 1: Batería
    {
      {VALUE_RPM, true, false},  // Voltaje batería
      {VALUE_BATTERY, true, false}     // Sin segundo valor
    },
    // Línea 2: Coolant
    {
      {VALUE_ENGINE_ASE, true, false},
      {VALUE_ENGINE_WARMUP, false, false}
    }
  },
  
  // Puedes agregar más páginas aquí
};

const uint8_t PAGE_COUNT = sizeof(PAGES) / sizeof(PAGES[0]);

// ========== DISPLAY CONFIG ==========

// Unidad por defecto para presiones
bool USE_PSI_DEFAULT = true;  // true = PSI, false = BAR

// Modo test por defecto
bool TEST_MODE_DEFAULT = true;

// Mensajes de inicio
const char* BOOT_MSG_LINE1 = "Hi Setterlee!";
const char* BOOT_MSG_LINE2 = "Welcome back :)";
const unsigned long BOOT_DELAY = 1200;

#endif
