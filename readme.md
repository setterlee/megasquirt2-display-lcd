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