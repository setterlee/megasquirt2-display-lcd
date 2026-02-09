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

## 🏗️ Arquitectura Modular

### Estructura del proyecto
```
ms2-display/
├── main.ino              # Loop principal, setup, UI
├── config.h              # Configuración de páginas y data sources
├── data_types.h          # Enums y estructuras de datos
├── sensor_direct.h       # Lectura de sensores directos (analog pins)
├── sensor_ms2.h          # Comunicación con MegaSquirt 2 (serial)
├── data_manager.h        # Gestor de datos (unifica sensores directos + MS2)
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
