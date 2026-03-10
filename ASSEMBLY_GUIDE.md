# 🔧 GUÍA DE ENSAMBLAJE - MS2 Display Monitor

## 📋 LISTA DE MATERIALES (BOM)

### Hardware Principal
- ✅ Arduino UNO (o compatible)
- ✅ LCD 16x2 I2C (dirección 0x27)
- ✅ Step-down 12V → 5V (mínimo 1A recomendado)
- ✅ 2x Pushbuttons (normalmente abiertos)
- ✅ Sensor NTC temperatura aceite
- ✅ Resistencia 10KΩ 1/4W (para divisor NTC)
- ✅ MegaSquirt 2 (ya instalado en el auto)

### Cables y Conectores
- Cable AWG 18-22 para 12V
- Cable AWG 22-24 para señales
- Conectores DB9 o compatible (para MS2 serial)
- Heat shrink / termocontraíble
- Cinta aislante
- Fusible 2A para línea de 12V

### Opcional pero Recomendado
- Caja/gabinete para Arduino + step-down
- Soporte para LCD (impresión 3D o comercial)
- Conectores JST/Dupont para facilitar desconexión
- LED indicador de power (opcional)

---

## 🔌 DIAGRAMA DE CONEXIONES

```
                    ┌─────────────────────────────────────┐
                    │          AUTO (12V Battery)          │
                    └──────────────┬──────────────────────┘
                                   │
                          ┌────────┴────────┐
                          │   [FUSIBLE 2A]   │
                          └────────┬────────┘
                                   │
                    ┌──────────────┴──────────────┐
                    │   STEP-DOWN 12V → 5V        │
                    │   Input: 12V                 │
                    │   Output: 5V @ 1A min       │
                    └──────────┬──────────────────┘
                               │ 5V
                    ┌──────────┴──────────┐
                    │    ARDUINO UNO      │
                    │                     │
                    │  5V ────┬──────────┼──── 5V Rail
                    │  GND ───┼──────────┼──── GND Rail
                    │         │          │
                    │  A2 ────┼──────────┼──── Temp Aceite (NTC + 10K)
                    │  A3 ────┼──────────┼──── Voltaje Batería (divisor)
                    │         │          │
                    │  D2 ────┼──────────┼──── Botón PAGE
                    │  D3 ────┼──────────┼──── Botón PEAK
                    │         │          │
                    │  A4 ────┼──────────┼──── LCD SDA (I2C)
                    │  A5 ────┼──────────┼──── LCD SCL (I2C)
                    │         │          │
                    │  TX ────┼──────────┼──── MS2 RX
                    │  RX ────┼──────────┼──── MS2 TX
                    └─────────┴──────────┘


┌──────────────────────────────────────────────────┐
│  SENSOR NTC TEMPERATURA ACEITE                   │
│                                                  │
│  5V ───[R 10K]───┬───[Sensor NTC]─── GND       │
│                  │                               │
│                  └─── Arduino A2                 │
└──────────────────────────────────────────────────┘


┌──────────────────────────────────────────────────┐
│  VOLTAJE BATERÍA (Divisor resistivo)            │
│                                                  │
│  12V ───[R 20K]───┬───[R 10K]─── GND           │
│                   │                              │
│                   └─── Arduino A3                │
│                                                  │
│  Opcional: Pueden usar el 12V directo del auto  │
│  (después del step-down toma una muestra)       │
└──────────────────────────────────────────────────┘


┌──────────────────────────────────────────────────┐
│  BOTONES                                         │
│                                                  │
│  D2 ───[BTN PAGE]─── GND    (pull-up interno)   │
│  D3 ───[BTN PEAK]─── GND    (pull-up interno)   │
│                                                  │
│  Botones normalmente abiertos, presión = GND    │
└──────────────────────────────────────────────────┘


┌──────────────────────────────────────────────────┐
│  LCD 16x2 I2C                                    │
│                                                  │
│  VCC ─── 5V                                      │
│  GND ─── GND                                     │
│  SDA ─── Arduino A4                              │
│  SCL ─── Arduino A5                              │
└──────────────────────────────────────────────────┘


┌──────────────────────────────────────────────────┐
│  MEGASQUIRT 2 (Serial)                           │
│                                                  │
│  MS2 TX ─── Arduino RX (pin 0)                   │
│  MS2 RX ─── Arduino TX (pin 1)                   │
│  MS2 GND ─── Arduino GND (común)                 │
│                                                  │
│  ⚠️  Verificar voltajes: MS2 usa 5V TTL          │
│  ⚠️  Puede que necesites un DB9 breakout         │
└──────────────────────────────────────────────────┘
```

---

## 🔨 PASOS DE ENSAMBLAJE

### PASO 1: Preparar el Arduino
1. Con Arduino DESCONECTADO de todo
2. Subir el firmware `ms2-display.ino` desde tu computadora
3. Verificar que compila sin errores
4. Desconectar del USB

### PASO 2: Conexiones de Power
1. **Step-down primero:**
   - Input del step-down: NO conectar aún al auto
   - Ajustar potenciómetro del step-down para 5.0V (con multímetro)
   - Output del step-down → Arduino 5V pin
   - GND del step-down → Arduino GND pin

2. **Verificar voltaje:**
   - Con el step-down alimentado temporalmente desde fuente de banco o batería
   - Medir con multímetro: Output debe ser 5.0V ± 0.2V
   - **IMPORTANTE**: Si es más de 5.3V, NO conectar Arduino

### PASO 3: Sensor NTC Temperatura Aceite
1. Soldar resistencia 10KΩ + cables al sensor NTC
2. Conexiones:
   ```
   5V (Arduino) ──[R 10K]──┬──[NTC]── GND
                           │
                        Pin A2
   ```
3. Aislar todas las conexiones con heat shrink
4. El sensor NTC se monta físicamente en el tapón del cárter o línea de aceite
5. **⚠️ IMPORTANTE**: Verificar que el sensor NO toque chasis (cortocircuito)

### PASO 4: Voltaje Batería (Monitor)
Para monitorear voltaje de batería del auto:

Opción A - Divisor resistivo (más preciso):
```
12V ──[R 20K]──┬──[R 10K]── GND
               │
            Pin A3
```
Esto convierte 0-20V → 0-5V para el Arduino

Opción B - Tap del step-down (más simple):
- Si tu step-down tiene pin de sense de voltaje de entrada, usalo
- Conectar a A3 con divisor apropiado

### PASO 5: LCD I2C
1. Conexiones LCD:
   - VCC → 5V
   - GND → GND  
   - SDA → A4
   - SCL → A5

2. **Verificar dirección I2C:**
   - La mayoría usa 0x27
   - Si no funciona, probar 0x3F
   - Cambiar en `config.h` si es necesario: `#define LCD_ADDRESS 0x27`

### PASO 6: Botones
1. Botón PAGE:
   - Un terminal → Pin D2
   - Otro terminal → GND
   
2. Botón PEAK:
   - Un terminal → Pin D3
   - Otro terminal → GND

3. No necesitan resistencias pull-up (Arduino las tiene internas)

### PASO 7: Conexión MegaSquirt 2
⚠️ **CRÍTICO**: Verificar pinout de tu MS2

Típico DB9 MegaSquirt:
- Pin 2: TX (salida MS2) → Arduino RX (pin 0)
- Pin 3: RX (entrada MS2) → Arduino TX (pin 1)  
- Pin 5: GND → Arduino GND

**ANTES de conectar:**
1. Medir voltaje TX del MS2 con multímetro (debe ser ~0-5V)
2. Verificar que GND es común entre MS2 y Arduino
3. Si MS2 usa 12V TTL, necesitás level shifter (raro en MS2)

---

## ⚡ CONEXIÓN AL AUTO

### Alimentación 12V
1. **Ubicación recomendada:** 
   - Tomar 12V de fusible ACC o IGN (se apaga con llave)
   - O directo de batería si querés que esté siempre encendido

2. **Conexión:**
   ```
   Batería(+) → [FUSIBLE 2A] → Step-down Input(+)
   Batería(-) → GND común → Step-down Input(-)
   ```

3. **⚠️ IMPORTANTE:**
   - SIEMPRE usar fusible (2A es suficiente)
   - Cables de calibre adecuado (18-20 AWG mínimo)
   - Verificar polaridad 3 veces antes de conectar

### Ubicación física
- Arduino + step-down: Dentro de gabinete, protegido del calor/humedad
- LCD: En tablero, visible para conductor
- Sensor NTC: En cárter o línea de aceite (verificar temperatura máxima del sensor)

### Ruteo de cables
- Cables de power: Separados de cables de señal
- Cables de sensores: Alejados de cables de chispa/bobinas (interferencia)
- Usar pasa cables y protección mecánica

---

## ✅ CHECKLIST PRE-ENCENDIDO

Antes de conectar al auto por primera vez:

### Verificaciones con multímetro (auto APAGADO):
- [ ] Step-down output = 5.0V ± 0.2V
- [ ] No hay cortocircuitos entre 5V y GND
- [ ] No hay cortocircuitos entre 12V y GND
- [ ] Resistencia NTC cambia con temperatura (medir con multímetro)
- [ ] Todos los cables están aislados
- [ ] Fusible instalado en línea de 12V

### Verificaciones visuales:
- [ ] Todas las conexiones soldadas/crimpeadas correctamente
- [ ] No hay cables pelados expuestos
- [ ] Arduino está montado (no flojo)
- [ ] LCD está montado y visible
- [ ] Botones son accesibles
- [ ] Sensor NTC aislado (no toca chasis)

### Primera alimentación (fuera del auto):
- [ ] Conectar step-down a fuente de 12V de banco
- [ ] Arduino debe encenderse (LED power)
- [ ] LCD debe mostrar mensaje de boot: "Hi Setterlee!"
- [ ] Botón PAGE cambia páginas
- [ ] Botón PEAK muestra min/max
- [ ] Temperatura aceite muestra valor (aunque sea ambiente)

### Primera conexión al auto:
- [ ] Auto APAGADO
- [ ] Conectar 12V del auto al step-down (CON FUSIBLE)
- [ ] Conectar GND común
- [ ] NO arrancar el auto todavía
- [ ] Solo girar llave a ACC: Arduino debe encenderse
- [ ] Verificar que LCD muestra datos
- [ ] Conectar cable serial a MS2
- [ ] Girar llave a ON (no arrancar): Verificar comunicación MS2
- [ ] Si todo OK, arrancar el auto

---

## 🔍 TROUBLESHOOTING

### Arduino no enciende
- Verificar voltaje step-down (debe ser 5.0V)
- Verificar fusible (no quemado)
- Verificar conexión de GND

### LCD no muestra nada
- Verificar contraste LCD (potenciómetro en módulo I2C)
- Verificar dirección I2C (probar 0x27 o 0x3F)
- Verificar alimentación 5V al LCD
- Verificar conexiones SDA/SCL

### LCD muestra cuadrados/basura
- Problema de contraste: ajustar potenciómetro
- O problema de alimentación: verificar 5V estable

### Temperatura aceite muestra valores raros
- Verificar resistencia 10K (medir con multímetro)
- Verificar que NTC no está en corto con chasis
- Con sensor a temperatura ambiente debe leer ~20-30°C

### MS2 no comunica (valores en 0)
- Verificar cables TX/RX (pueden estar invertidos)
- Verificar baudrate en MS2 (debe ser 9600)
- Verificar GND común entre MS2 y Arduino
- En modo test debe funcionar (cambiar `TEST_MODE_DEFAULT = true`)

### Presiones muestran 0 o valores raros
- Verificar que MS2 está configurado correctamente (ADC6, ADC7)
- Verificar custom.ini está cargado en MS2
- Verificar sensores de presión conectados a MS2

### Arduino se resetea constantemente
- Step-down no entrega suficiente corriente (mínimo 1A)
- Verificar que no hay cortocircuitos
- Verificar que 12V es estable (no cae al arrancar)

---

## ⚠️ ADVERTENCIAS DE SEGURIDAD

1. **Electrical:**
   - SIEMPRE desconectar batería antes de trabajar
   - Usar fusible apropiado (2A recomendado)
   - No trabajar con motor en marcha al hacer conexiones

2. **Sensor NTC:**
   - Verificar temperatura máxima del sensor
   - Instalar en lugar accesible para mantenimiento
   - Usar thread sealant apropiado (no Teflon en sensores)

3. **Montaje:**
   - No montar cerca de fuentes de calor extremo
   - Proteger de agua/humedad
   - Cables no deben interferir con pedales/palanca

4. **Durante pruebas:**
   - Tener extintor a mano
   - Primera prueba con auto en lugar seguro/abierto
   - Monitorear temperatura del step-down (no debe calentar mucho)

---

## 📱 CONFIGURACIÓN FINAL

Una vez todo conectado y funcionando:

1. **Ajustar TEST_MODE:**
   - En `config.h`: `TEST_MODE_DEFAULT = false`
   - Re-compilar y subir

2. **Verificar alertas:**
   - En `config.h`: `ENABLE_ALERTS = true` (cuando estés listo)
   - Probar que alertas funcionan correctamente

3. **Personalizar páginas:**
   - Editar `PAGES[]` en `config.h` según preferencias
   - Cambiar qué valores se muestran en cada página

4. **Calibrar sensores:**
   - Si valores no son exactos, ajustar rangos en `config.h`
   - Para NTC ya está calibrado, pero verificar

---

## 📊 VALORES ESPERADOS (Motor en marcha, ralentí)

| Sensor | Rango normal ralentí | Rango normal en marcha |
|--------|---------------------|----------------------|
| Temp aceite | 80-95°C | 95-110°C |
| Presión aceite | 20-40 PSI | 40-60 PSI |
| Presión combustible | 40-55 PSI | 40-55 PSI |
| MAP | 30-50 kPa (vacío) | 50-300 kPa (boost) |
| RPM | 800-1000 | 1000-7000 |
| Voltaje batería | 13.5-14.5V | 13.5-14.5V |
| Temp refrigerante | 85-95°C | 85-95°C |

---

## 🎉 ¡Listo para rodar!

Una vez verificado todo, tu sistema está listo para uso diario.

**Recomendaciones:**
- Primeras salidas: monitorear todo de cerca
- Anotar valores normales de tu motor específico
- Ajustar alertas según tu setup
- Enjoy responsibly! 🏁
