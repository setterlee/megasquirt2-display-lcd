# 🛒 LISTA DE MATERIALES (BOM) - MS2 Display Monitor

## 📦 COMPONENTES PRINCIPALES

### Microcontrolador
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Arduino UNO R3 | Original o compatible | $20-30 USD | Amazon, AliExpress, MercadoLibre |

**Alternativas compatibles:**
- Arduino UNO clone (más barato, funciona igual)
- Arduino Nano (más compacto, necesita adaptar pines)

---

### Display
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | LCD 16x2 I2C | Con backpack I2C incluido, backlight azul/verde | $5-10 USD | Amazon, AliExpress, MercadoLibre |

**Verificar:**
- Dirección I2C: 0x27 o 0x3F (típico)
- Voltaje: 5V
- Backlight: LED (no incandescente)
- Incluye módulo I2C soldado (4 pines: VCC, GND, SDA, SCL)

**Alternativas:**
- LCD 20x4 I2C (más grande, más info visible)
- OLED 128x64 I2C (mejor contraste, requiere modificar código)

---

### Power Supply
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Step-down Buck Converter | 12V → 5V, min 1A output | $2-5 USD | Amazon, AliExpress |

**Modelos recomendados:**
- LM2596 DC-DC Buck converter (típico, barato)
- MP1584EN (muy compacto)
- XL4015 (para más corriente si expandís después)

**Especificaciones mínimas:**
- Input: 6-35V (12V auto OK)
- Output: 5V ajustable
- Corriente: mínimo 1A (recomendado 2A)
- Eficiencia: >90%

---

### Sensores
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Sensor NTC Temperatura | Para aceite, rango 20-150°C | $5-15 USD | Tienda auto, AliExpress |

**Tu sensor ya lo tenés:**
- Resistencia: 62.32KΩ @ 20°C → 0.90KΩ @ 150°C
- Típico: VDO, Autometer, o sensor genérico auto
- Rosca: 1/8" NPT o M10x1.0 típico
- Cables: 2 hilos

**Si necesitás comprar uno nuevo:**
- Buscar: "NTC Temperature Sensor Oil"
- Verificar curva de resistencia vs temperatura
- Que sea para aceite (rango hasta 150°C)

---

### Botones
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 2 | Pushbutton momentary | Normalmente abierto (NO), 12mm típico | $0.50 c/u | Electrónica local, AliExpress |

**Opciones:**
- Pushbutton panel mount (12mm o 16mm)
- Botones OEM estilo auto (más profesional)
- Buttons arcade style (si querés algo distintivo)

**Especificaciones:**
- Tipo: Momentáneo (momentary), NO (normally open)
- Voltaje: 12V o más
- Corriente: >50mA (bajo consumo, cualquiera sirve)

---

## 🔌 COMPONENTES ELÉCTRICOS

### Resistencias
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Resistencia 10KΩ | 1/4W, ±5% tolerancia | $0.10 | Electrónica local |
| 1 | Resistencia 20KΩ | 1/4W, ±5% (opcional, para vbat) | $0.10 | Electrónica local |

**Para divisor NTC:**
- 1x 10KΩ 1/4W (OBLIGATORIO)

**Para divisor voltaje batería (opcional):**
- 1x 20KΩ 1/4W
- 1x 10KΩ 1/4W

---

### Cables y Conectores
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 2-3m | Cable AWG 22-24 | Para señales (varios colores) | $5/rollo | Electrónica local |
| 1-2m | Cable AWG 18-20 | Para power 12V | $3/m | Electrónica local |
| 1 | Cable DB9 o serial | Para conectar a MS2 | $5-10 | Electrónica, auto |
| 5-10 | Conectores Dupont/JST | Hembra/Macho según necesidad | $5/kit | AliExpress |
| 1 | Heat shrink kit | Varios diámetros | $10/kit | Electrónica, Amazon |

**Recomendaciones:**
- Colores: Rojo (12V), Negro (GND), Otros para señales
- Cable multinúcleo flexible (no sólido)
- Heat shrink con adhesivo (mejor sellado)

---

### Protección
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Fusible Blade 2A | Tipo ATO/ATC | $1 | Tienda auto |
| 1 | Portafusibles inline | Para fusible blade | $2-5 | Tienda auto |

**Alternativa:**
- Fusible de vidrio 2A + portafusibles inline

---

## 🔧 HERRAMIENTAS Y ACCESORIOS

### Montaje
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Gabinete plástico | 10x8x5cm aprox, con ventilación | $5-10 | Electrónica, ferretería |
| 1 | Soporte LCD | Custom o comercial | Variable | Imprimir 3D / Tienda auto |
| 1 | Velcro industrial | Adhesivo fuerte, 5cm | $5 | Ferretería |
| 20-30 | Zip ties / Bridas | Varios tamaños | $5/100 | Ferretería |
| 1 | Adaptador sensor | Para instalar NTC (depende ubicación) | $10-30 | Tienda auto |

**Adaptadores comunes para sensor NTC:**
- Tapón cárter con puerto 1/8" NPT
- Sandwich plate filtro aceite con puerto sensor
- T-fitting para línea de presión

---

### Consumibles
| Qty | Componente | Especificación | Precio aprox | Dónde comprar |
|-----|-----------|----------------|--------------|---------------|
| 1 | Thread sealant | Para sensor NTC (NO Teflon) | $5 | Tienda auto |
| 1 | Silicona/sellador | Impermeabilización | $5 | Ferretería |
| 1 | Cinta aislante | Eléctrica | $3 | Ferretería |
| 1 | Grasa dieléctrica | Para ground/conexiones | $5 | Tienda auto/electrónica |

---

## 🛠️ HERRAMIENTAS NECESARIAS

### Básicas (probablemente ya tengas)
- [ ] Soldador eléctrico (30-60W)
- [ ] Estaño (resina core)
- [ ] Alicate de corte
- [ ] Alicate de punta
- [ ] Destornilladores (plano y Phillips)
- [ ] Multímetro digital
- [ ] Pelacables / Wire stripper

### Recomendadas
- [ ] Pistola de calor (para heat shrink)
- [ ] Crimper para terminales
- [ ] Drill + brocas (para montaje)
- [ ] Llaves/dados (instalación sensor)

---

## 💰 PRESUPUESTO TOTAL

### Opción Mínima (lo esencial)
```
Arduino UNO clone        $15
LCD 16x2 I2C            $8
Step-down LM2596        $3
Resistencia 10K         $0.10
Botones (2x)            $2
Cables varios           $10
Fusible + holder        $3
Heat shrink             $5
Gabinete/soporte        $10
Consumibles             $10
─────────────────────────────
TOTAL:                  ~$65 USD
```

### Opción Completa (profesional)
```
Arduino UNO original    $25
LCD 16x2 I2C quality    $12
Step-down MP1584        $5
Sensor NTC nuevo        $15
Resistencias            $1
Botones premium         $10
Cables quality          $20
Conectores waterproof   $15
Fusible blade + holder  $5
Heat shrink kit         $15
Gabinete custom         $15
Soporte LCD             $20
Adaptador sensor        $25
Herramientas extras     $30
Consumibles             $20
─────────────────────────────
TOTAL:                  ~$230 USD
```

**Tu proyecto actual (sensor ya tienes):**
Estimado: **$50-80 USD** (teniendo ya herramientas básicas)

---

## 🌐 DÓNDE COMPRAR

### Internacional (shipping a Argentina puede tardar)
- **AliExpress**: Más barato, tarda 30-60 días
- **Amazon**: Más rápido (7-15 días), más caro
- **eBay**: Similar a AliExpress

### Argentina (más rápido, posible más caro)
- **MercadoLibre**: Todo disponible, 24-48hs
- **Electrónica:
  * **Electrodacta** (CABA)
  * **DigiKey** (online, stock confiable)
  * **Electrocomponentes** (CABA)
- **Auto parts:**
  * Casa de repuestos locales (sensores, adaptadores)
  * Tiendas tuning (adaptadores performance)

### Tips de compra:
1. **Armar lista completa ANTES de comprar**
2. **Comprar todo junto** (shipping único si es online)
3. **Comprar extras** de componentes baratos (resistencias, fusibles)
4. **Verificar compatibilidad** antes de comprar (especialmente adaptadores)
5. **Leer reviews** (especialmente step-down, calidad varía)

---

## 📋 CHECKLIST DE COMPRA

### Antes de ir a comprar:
- [ ] Imprimir esta lista o tenerla en el celular
- [ ] Medir espacio disponible en auto (para gabinete/LCD)
- [ ] Verificar qué rosca tiene tu motor para sensor (1/8" NPT, M10, etc)
- [ ] Calcular largo de cables necesarios
- [ ] Ver qué herramientas ya tenés

### En la tienda:
- [ ] Verificar voltaje componentes (5V o 12V según corresponda)
- [ ] Verificar que LCD tenga módulo I2C soldado
- [ ] Verificar step-down tenga potenciómetro ajustable
- [ ] Comprar cables de colores (facilita instalación)
- [ ] Comprar fusibles extra (por si uno se quema)

---

## 🔄 COMPONENTES OPCIONALES (Expansión futura)

### Para mejorar el proyecto:
| Componente | Uso | Precio aprox |
|-----------|-----|--------------|
| Buzzer piezo | Alertas sonoras | $2 |
| LED RGB | Indicador visual alertas | $1 |
| Potenciómetro | Ajuste brillo LCD | $2 |
| SD card module | Datalogging | $5 |
| Bluetooth HC-05 | App móvil | $8 |
| RTC DS3231 | Timestamp datos | $3 |

### Sensores adicionales (pines libres A0, A1):
| Sensor | Uso | Precio aprox |
|--------|-----|--------------|
| Sensor presión boost | MAP adicional | $30 |
| Sensor EGT K-type | Temp gases escape | $25 |
| Sensor flex fuel | Contenido etanol | $50 |
| Sensor wide-band O2 | AFR preciso | $150+ |

---

## 📱 ANTES DE COMPRAR - VERIFICAR

1. **Compatibilidad voltaje:**
   - Arduino: 5V ✓
   - LCD: 5V ✓
   - Sensor NTC: Pasivo (cualquier voltaje) ✓
   - Step-down input: 12V ✓

2. **Capacidad corriente:**
   - Total consumo: ~200mA
   - Step-down mínimo: 1A (margen 5x)
   - Fusible: 2A (margen 10x)

3. **Espacio físico:**
   - Arduino UNO: 68.6mm x 53.4mm
   - LCD 16x2: 80mm x 36mm
   - Step-down: ~43mm x 21mm (típico LM2596)

4. **Tu MS2:**
   - Firmware: MS2 Extra 3.4.4 ✓
   - Serial: 9600 baud ✓
   - ADC6, ADC7 configurados ✓

---

## ✅ RESUMEN EJECUTIVO

**Para empezar necesitás (mínimo):**
1. Arduino UNO ($15-25)
2. LCD 16x2 I2C ($8-12)
3. Step-down 12V→5V ($3-5)
4. Resistencia 10K ($0.10)
5. 2x Botones ($2)
6. Cables + conectores ($10-15)
7. Fusible 2A + holder ($3-5)
8. Heat shrink ($5)
9. Gabinete ($5-10)

**TOTAL: $50-80 USD**

Tu sensor NTC ya lo tenés ✓
Tu MS2 ya está configurado ✓
Herramientas básicas (soldador, multímetro) asumo que tenés ✓

**Tiempo de instalación:**
- Ensamblaje banco: 2-3 horas
- Instalación en auto: 3-5 horas
- Ajustes finales: 1 hora

**TOTAL: 1 día de trabajo tranquilo**

---

¡Listo para pedir las piezas! 🛒🏁
