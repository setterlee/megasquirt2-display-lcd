# Lancer Custom Monitor – Arduino UNO + LCD I2C

Monitor auxiliar retro para Mitsubishi Lancer basado en **Arduino UNO** y **LCD 16x2 I2C**, pensado para uso automotriz real:
- discreto
- rápido de leer
- sin llenar el tablero de relojes

Incluye:
- paginación por botón
- registro de mínimos y máximos (MIN / MAX)
- modo TEST para validación sin motor
- **arquitectura modular y configurable**
- **comunicación con MegaSquirt 2**
- **sensores directos + MS2 unificados**

---

## 📑 Índice

1. [Arquitectura Modular](#-arquitectura-modular)
2. [Hardware](#-hardware)
3. [Conexiones](#-conexiones)
4. [Configuración (config.h)](#%EF%B8%8F-configuración-configh)
   - [Definir valores disponibles](#definir-valores-disponibles)
   - [Configurar páginas](#configurar-páginas)
5. [Sensores y Valores Disponibles](#-sensores-y-valores-disponibles)
   - [Sensores directos](#sensores-directos-analógicos)
   - [Valores desde MS2](#valores-desde-megasquirt-2)
   - [Engine Status Flags](#engine-status-flags)
6. [Sistema de Alertas](#-sistema-de-alertas)
   - [Alertas simples (rangos)](#alertas-simples-rangos)
   - [Alertas compuestas](#alertas-compuestas)
   - [Casos de prueba en Test Mode](#casos-de-prueba-en-test-mode)
7. [Funcionalidad](#-funcionalidad)
8. [Fuentes de datos](#-fuentes-de-datos)
9. [Agregar nuevo valor](#-cómo-agregar-un-nuevo-valor)
10. [Ventajas](#-ventajas-de-la-arquitectura-modular)

---

## 🏗️ Arquitectura Modular

### Estructura del proyecto
```
ms2-display/
├── ms2-display.ino       # Loop principal, setup, UI
├── config.h              # Configuración de páginas y data sources
├── data_types.h          # Enums y estructuras de datos
├── sensor_direct.h       # Lectura de sensores directos (analog pins)
├── sensor_ms2.h          # Comunicación con MegaSquirt 2 (serial)
├── data_manager.h        # Gestor de datos (unifica sensores directos + MS2)
├── alert_manager.h       # Sistema de alertas (rangos + compuestas)
└── display_helper.h      # Funciones auxiliares de display
```

### Filosofía de diseño
- **Configuración centralizada**: Todo se define en `config.h`
- **Fuentes de datos intercambiables**: Cada valor puede venir de sensor directo o MS2
- **Páginas customizables**: Agregar/modificar páginas sin tocar código
- **Extensible**: Fácil agregar nuevos sensores o protocolos (CAN, ESP32, etc.)

---

## 📦 Hardware

### Microcontrolador
- Arduino UNO (ATmega328P)

### Display
- LCD 16x2 con backpack **I2C**
- Librería: `LiquidCrystal_I2C`
- Dirección típica: `0x27` (ajustable)

### Botones
- 2 × pulsadores momentáneos (NA)
- Conectados a GND
- Pull-up interno activado por software

### Sensores directos (analógicos)
- Presión de aceite → A1
- Temperatura de aceite → A2
- Voltaje batería → A3 (divisor resistivo)
- Temperatura de aire → A6

### MegaSquirt 2
- Puerto Serial → D0 (RX) / D1 (TX)
- Baud rate: 9600
- Protocolo: MS2 Extra realtime data
- Datos disponibles: MAP, IAT, CLT, RPM, TPS, AFR, ignition timing, etc.

---

## 🔌 Conexiones

### LCD I2C
| Señal | Arduino |
|---|---|
| SDA | A4 |
| SCL | A5 |
| VCC | 5V |
| GND | GND |

> Nota: A4 y A5 quedan ocupados por I2C (no como ADC)

---

### Sensores Directos
| Sensor | Pin |
|---|---|
| Presión aceite | A1 |
| Temp aceite | A2 |
| Batería | A3 |
| Temp aire | A6 |

---

### MegaSquirt 2
| Señal | Arduino |
|---|---|
| TX MS2 | RX (D0) |
| RX MS2 | TX (D1) |
| GND | GND común |

---

### Botones
| Función | Pin |
|---|---|
| PAGE | D2 |
| PEAK (MIN/MAX) | D3 |

Conexión:
```
D2 ----[ BOTÓN PAGE ]---- GND
D3 ----[ BOTÓN PEAK ]---- GND
```

Configurados con `INPUT_PULLUP`.

---

## ⚙️ Configuración (config.h)

### Definir valores disponibles
```cpp
const ValueConfig VALUE_CONFIGS[] = {
  // type              source          label   unit          pin  minRaw maxRaw  minReal maxReal decimals
  {VALUE_MAP,          SOURCE_MS2,     "MAP",  UNIT_PSI,     0,   0,     1023,   -14.5,  30.0,   1},
  {VALUE_OIL_PRESSURE, SOURCE_DIRECT,  "OIL",  UNIT_PSI,     A1,  0,     1023,   0.0,    87.0,   1},
  {VALUE_OIL_TEMP,     SOURCE_DIRECT,  "OLT",  UNIT_CELSIUS, A2,  0,     1023,   20.0,   140.0,  0},
  // ... más valores
};
```

**Parámetros de ValueConfig:**
- `type`: Tipo de valor (VALUE_MAP, VALUE_OIL_PRESSURE, etc.)
- `source`: Fuente de datos
  - `SOURCE_DIRECT` = sensor analógico directo
  - `SOURCE_MS2` = dato desde MegaSquirt 2
  - `SOURCE_TEST` = valor simulado
- `label`: Etiqueta corta para LCD (3-4 caracteres)
- `unit`: Unidad de medida (UNIT_PSI, UNIT_CELSIUS, UNIT_VOLT, etc.)
- `pin`: Pin analógico (solo para SOURCE_DIRECT, ej: A1, A2, A3)
- `minRaw/maxRaw`: Rango del ADC (0-1023 para Arduino UNO)
- `minReal/maxReal`: Rango real después de calibración
- `decimals`: Cantidad de decimales a mostrar (0, 1, o 2)

**Ejemplo de calibración:**
```cpp
// Sensor de presión 0-100 PSI conectado a A1
// ADC 0 = 0 PSI, ADC 1023 = 100 PSI
{VALUE_OIL_PRESSURE, SOURCE_DIRECT, "OIL", UNIT_PSI, A1, 0, 1023, 0.0, 100.0, 1}
```

---

### Configurar páginas
```cpp
const PageConfig PAGES[] = {
  // PAGE 0: MAIN
  {
    // Línea 1: MAP + Temp aire
    {
      {VALUE_MAP, true, true},      // con unidad y signo +/-
      {VALUE_AIR_TEMP, true, false} // con unidad
    },
    // Línea 2: Presión aceite + Temp aceite
    {
      {VALUE_OIL_PRESSURE, true, false},
      {VALUE_OIL_TEMP, true, false}
    }
  },
  // Agregar más páginas...
};
```

**Formato de DisplayItem**: `{ValueType, showUnit, showSign}`

**Parámetros:**
- `showUnit` (bool): Si mostrar la unidad después del valor
  - `true` → "MAP:+17.4**p**", "60**C**", "14.5**V**"
  - `false` → "MAP:+17.4", "60", "14.5"

- `showSign` (bool): Si mostrar signo `+` en valores positivos
  - `true` → "MAP:**+**17.4p" (muestra + y -)
  - `false` → "MAP:17.4p" (solo muestra - cuando es negativo)

**Ejemplos:**
```cpp
{VALUE_MAP, true, true}    // "MAP:+17.4p" o "MAP:-8.7p" (con unidad, con signo)
{VALUE_MAP, true, false}   // "MAP:17.4p" o "MAP:-8.7p"  (con unidad, sin + para positivos)
{VALUE_MAP, false, false}  // "MAP:17.4" o "MAP:-8.7"    (sin unidad, sin signo)
{VALUE_OIL_TEMP, true, false}  // "92C" (temperatura, siempre positiva, no necesita +)
```

**¿Cuándo usar `showSign=true`?**
- En valores que pueden ser positivos o negativos (MAP/boost)
- Cuando quieres distinguir rápidamente dos estados (vacío vs boost)

---

## 📊 Sensores y Valores Disponibles

### Sensores directos (analógicos)
Conectados directamente a pines del Arduino:

| Sensor | Pin | Rango | Unidad |
|--------|-----|-------|--------|
| Presión de aceite | A1 | 0-87 PSI | PSI |
| Temp. aceite | A2 | 20-140°C | Celsius |
| Batería | A3 | 0-20V | Volt |

### Valores desde MegaSquirt 2
Leídos por protocolo serial (9600 baud):

| Valor | Label | Rango | Unidad | Descripción |
|-------|-------|-------|--------|-------------|
| MAP | MAP | -14.5 a 30 PSI | PSI | Presión absoluta (boost/vacío) |
| Air Temp (IAT) | IAT | -10 a 80°C | Celsius | Temperatura aire admisión |
| Coolant Temp | CLT | 20 a 120°C | Celsius | Temperatura refrigerante |
| RPM | RPM | 0-8000 | RPM | Revoluciones por minuto |
| TPS | TPS | 0-100 | % | Posición acelerador |
| AFR | AFR | 10.0-20.0 | Ratio | Relación aire/combustible |
| Ignition | IGN | -10 a 50 | Grados | Avance de ignición |
| Dwell | DWL | 0-10 | ms | Tiempo de carga bobina |
| Fuel Pressure | FUP | 0-100 | PSI | Presión de combustible |
| Pulse Width | PW1 | 0-25.5 | ms | Duración inyección |
| Engine Status | STA | - | Flags | Estado del motor (ver abajo) |

### Engine Status Flags
El valor `ENGINE_STATUS` decodifica el byte de estado del MS2 mostrando indicadores activos:

**Flags disponibles:**
- **RDY** = Ready (motor listo)
- **CRK** = Cranking (arrancando)
- **ASE** = After Start Enrichment (enriquecimiento post arranque)
- **WUE** = Warmup Enrichment (enriquecimiento en calentamiento)
- **TPS** = TPS Acceleration Enrichment (aceleración)
- **LCH** = Launch Control (control de largada)
- **FSH** = Flat Shift / Spark Cut (corte de chispa)

**Formato en LCD:**
```
RDY ASE TPS     ← Motor listo, post arranque, acelerando
RDY LCH FSH     ← Launch control activo, flat shift
```

**Uso típico:**
```cpp
{VALUE_ENGINE_STATUS, false, false}  // Ocupa línea completa con flags activos
```

**Valores normales esperados:**

**Ralentí (800 RPM):**
- Fuel Pressure: 40-45 PSI
- Pulse Width: 2-3 ms
- Status: RDY (+ WUE si motor frío)

**Aceleración moderada (3000 RPM, 50% TPS):**
- Fuel Pressure: 43-47 PSI
- Pulse Width: 5-7 ms
- Status: RDY TPS

**Aceleración fuerte (5000 RPM, 100% TPS):**
- Fuel Pressure: 45-50 PSI
- Pulse Width: 8-12 ms
- Status: RDY TPS

---

## 🚨 Sistema de Alertas

El sistema de alertas monitorea continuamente los sensores y activa el **titilado del backlight** cuando detecta condiciones peligrosas.

### Alertas simples (rangos)

Definidas en `ALERT_RANGES[]` en [config.h](config.h), monitorean valores fuera de rango:

| Sensor | Min | Max | Severidad | Descripción |
|--------|-----|-----|-----------|-------------|
| OIL_PRESSURE | 10 PSI | - | 🚨 CRITICAL | Presión de aceite críticamente baja |
| OIL_TEMP | - | 125°C | ⚠️ WARNING | Temperatura aceite alta |
| OIL_TEMP | - | 135°C | 🚨 CRITICAL | Temperatura aceite crítica |
| COOLANT_TEMP | - | 105°C | ⚠️ WARNING | Temperatura coolant alta |
| COOLANT_TEMP | - | 115°C | 🚨 CRITICAL | Temperatura coolant crítica |
| BATTERY | 11.5V | 15.5V | ⚠️ WARNING | Voltaje fuera de rango |
| AFR | 11.0 | 16.0 | ⚠️ WARNING | Mezcla muy rica o muy pobre |

**Velocidad de titilado:**
- ⚠️ **WARNING**: 500ms (1 parpadeo por segundo)
- 🚨 **CRITICAL**: 200ms (2.5 parpadeos por segundo)

### Alertas compuestas

Verifican **combinaciones de valores** para detectar situaciones peligrosas:

#### 1. Motor frío + carga alta
```cpp
if (COOLANT_TEMP < 85°C && OIL_TEMP < 85°C) 
   && (TPS > 50% || MAP > 5 PSI)
→ ⚠️ WARNING: No aceleres fuerte con motor frío
```

**Por qué es peligroso:**
- Aceite más viscoso → menor lubricación
- Desgaste prematuro de cilindros y anillos
- Mayor consumo de combustible

#### 2. Baja presión aceite + RPM altas
```cpp
if (OIL_PRESSURE < 10 PSI && RPM > 2000)
→ 🚨 CRITICAL: Posible falla de bomba o nivel bajo
```

**Por qué es peligroso:**
- Lubricación insuficiente en componentes críticos
- Riesgo de fundición de bielas/cojinetes
- Daño catastrófico del motor

#### 3. Temperaturas combinadas altas
```cpp
if (COOLANT_TEMP > 105°C && OIL_TEMP > 120°C)
→ 🚨 CRITICAL: Sistema de enfriamiento comprometido
```

**Por qué es peligroso:**
- Riesgo de ebullición del coolant
- Degradación acelerada del aceite
- Posible detonación por alta temperatura

### Casos de prueba en Test Mode

El **Test Mode** simula 4 escenarios en ciclo para verificar las alertas:

#### 📊 Ciclo de prueba (testStep 0.0 → 1.0)

**Paso 1: Motor frío + aceleración** (0.0 - 0.2)
```
COOLANT: 65°C  OIL: 70°C  TPS: 60%  MAP: +6 PSI
→ ⚠️ Alerta: COLD_ENGINE_HIGH_LOAD
→ Backlight: Titila lento (500ms)
```

**Paso 2: Baja presión aceite** (0.2 - 0.4)
```
OIL_PRESSURE: 8 PSI  RPM: 4000
→ 🚨 Alerta: LOW_OIL_PRESSURE
→ Backlight: Titila rápido (200ms)
```

**Paso 3: Sobrecalentamiento** (0.4 - 0.6)
```
COOLANT: 110°C  OIL: 130°C
→ 🚨 Alerta: HIGH_TEMP_COMBO
→ Backlight: Titila rápido (200ms)
```

**Paso 4: Operación normal** (0.6 - 1.0)
```
Todos los valores en rangos seguros
→ ✅ Sin alertas
→ Backlight: Siempre encendido
```

### Configurar alertas

**Deshabilitar una alerta:**
```cpp
{VALUE_OIL_TEMP, ALERT_WARNING, 40.0, 125.0, false},  // enabled = false
```

**Cambiar umbrales:**
```cpp
{VALUE_OIL_PRESSURE, ALERT_CRITICAL, 15.0, 87.0, true},  // min: 10→15 PSI
```

**Agregar nueva alerta compuesta en [alert_manager.h](alert_manager.h):**
```cpp
bool checkMyCustomAlert() {
  float value1 = dataManager->getValue(VALUE_XXX);
  float value2 = dataManager->getValue(VALUE_YYY);
  return (value1 > threshold && value2 < threshold);
}

// Agregar en update():
if (checkMyCustomAlert()) {
  alertActive = true;
  currentSeverity = ALERT_WARNING;
}
```

---

## 🧠 Funcionalidad

### Páginas

#### Página principal (sensores directos + MS2)
```
MAP:+11.6p 25C
OIL:60.9p 92C
```

#### Página batería
```
BAT:13.8V
MODE:TEST
```

---

### Botón PAGE
- Click corto → cambia de página
- Click largo (≥1.5 s) → cambia unidad BAR ↔ PSI

---

### Botón PEAK
- Click corto → muestra **MIN / MAX** de la página actual (3 s)
- Click largo (≥1.5 s) → resetea MIN / MAX

Formato MIN / MAX:
```
MAP -8.7/+17.4
OIL 14.5/73.1p
```

---

## 🧪 Modo TEST

- Valores simulados (oscilantes)
- Permite validar sin motor ni MS2 conectado
- Se indica en pantalla como `MODE:TEST`
- Configurable en `config.h`: `TEST_MODE_DEFAULT = true`

---

## 📊 Fuentes de datos

### SOURCE_DIRECT
- Lee directamente de pines analógicos
- Calibración en `config.h`
- No requiere MS2

### SOURCE_MS2
- Lee desde MegaSquirt 2 por serial
- Parsea protocolo binario MS2 Extra
- Fallback automático si no hay conexión

### SOURCE_TEST
- Valores simulados oscilantes
- Útil para desarrollo y testing

---

## 🔧 Cómo agregar un nuevo valor

1. **Definir enum** en `data_types.h`:
```cpp
enum ValueType {
  // ...
  VALUE_FUEL_PRESSURE,  // Nuevo valor
};
```

2. **Configurar** en `config.h`:
```cpp
{VALUE_FUEL_PRESSURE, SOURCE_DIRECT, "FUL", UNIT_PSI, A7, 0, 1023, 0.0, 100.0, 1},
```

3. **Agregar a página** en `config.h`:
```cpp
{VALUE_FUEL_PRESSURE, true, false}
```

Listo! El sistema lo maneja automáticamente.

---

## 🚀 Ventajas de la arquitectura modular

✅ **Configuración sin código**: Cambia fuentes de datos editando solo `config.h`
✅ **Páginas ilimitadas**: Agrega cuantas necesites
✅ **Testing independiente**: Sensores directos funcionan sin MS2
✅ **Extensible**: Fácil agregar CAN bus, ESP32, más protocolos
✅ **Mantenible**: Cada módulo tiene una responsabilidad clara
✅ **Reutilizable**: Los valores se pueden mostrar en múltiples páginas

---
* ADC externo (ADS1115) si se requieren más canales

---

## ⚠️ Notas automotrices

* Usar buena masa común (sensores + Arduino)
* Filtrar señales analógicas si vienen del vano motor
* Para batería: usar divisor resistivo adecuado (máx 14.5 V)
* No alimentar desde USB en el auto (usar regulador 12V→5V)

---

## 🧠 Filosofía

Este proyecto prioriza:

* **legibilidad**
* **mínima distracción**
* **información crítica**
* **estética retro funcional**

No busca reemplazar la ECU, sino **complementarla**.

---

## 📜 Licencia

Proyecto personal / experimental.
Usar bajo su propio criterio y responsabilidad.

```
