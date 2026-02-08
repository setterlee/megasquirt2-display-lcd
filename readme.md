# Lancer Custom Monitor – Arduino UNO + LCD I2C

Monitor auxiliar retro para Mitsubishi Lancer basado en **Arduino UNO** y **LCD 16x2 I2C**, pensado para uso automotriz real:
- discreto
- rápido de leer
- sin llenar el tablero de relojes

Incluye:
- paginación por botón
- registro de mínimos y máximos (MIN / MAX)
- modo TEST para validación sin motor
- arquitectura extensible (alarmas, más páginas, ESP32 futuro)

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

### Sensores (analógicos)
- MAP / Boost
- Presión de aceite
- Temperatura de aceite
- Voltaje batería (divisor resistivo)

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

### Sensores
| Sensor | Pin |
|---|---|
| MAP | A0 |
| Presión aceite | A1 |
| Temp aceite | A2 |
| Batería | A3 |

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

## 🧠 Funcionalidad

### Páginas

#### Página principal
```

MAP:+0.8b
OIL:4.2b 92C

```

#### Página batería
```

BAT:13.8V
MODE:REAL

```

---

### Botón PAGE
- Click corto → cambia de página

---

### Botón PEAK
- Click corto → muestra **MIN / MAX** de la página actual (3 s)
- Click largo (≥1.5 s) → resetea MIN / MAX

Formato MIN / MAX (mínimo a la izquierda):
```

MAP -0.6/+1.2
OIL 1.0/5.1b

````

---

## 🧪 Modo TEST

- Valores simulados (oscilantes)
- Permite validar:
  - layout
  - legibilidad
  - refresco
  - lógica de picos
- Se indica en pantalla como `MODE:TEST`

Ideal para pruebas sin motor.

---

## ⏱️ Tiempos clave (configurables)

```cpp
UPDATE_INTERVAL = 250 ms
TEST_INTERVAL   = 120 ms
PEAK_VIEW_TIME  = 3000 ms
LONG_PRESS_TIME = 1500 ms
````

---

## 📚 Librerías requeridas

Instalar desde el Library Manager:

* `LiquidCrystal_I2C`
* `Wire` (incluida en Arduino core)

---

## 🧱 Arquitectura del código

* Lectura de sensores separada de display
* Picos (MIN / MAX) independientes de páginas
* Sistema de páginas escalable (`enum Page`)
* Botones con:

  * debounce implícito
  * detección de click corto / largo

---

## 🚀 Extensiones previstas

Fácil de agregar:

* alarmas visuales (parpadeo / latch)
* buzzer
* más páginas
* lectura serial desde MegaSquirt
* migración a ESP32
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
