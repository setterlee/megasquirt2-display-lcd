# 🚗 INSTALACIÓN EN VEHÍCULO - Tips y Consideraciones

## 📍 UBICACIÓN DE COMPONENTES

### Arduino + Step-down (Módulo Principal)
**Ubicación recomendada:**
- Debajo del tablero (lado conductor o pasajero)
- Dentro de la cabina (protegido de intemperie)
- Lejos de fuentes de calor intenso (motor, escape)
- Accesible para mantenimiento

**Consideraciones:**
- ✅ Temperatura ambiente: -10°C a +70°C (típico Arduino)
- ✅ Montar en gabinete plástico o metálico (con ventilación)
- ✅ Usar velcro industrial o tornillos para fijación
- ⚠️ NO montar directamente sobre metal (riesgo cortocircuito)
- ⚠️ NO cerca de ECU/BCM (interferencia electromagnética)

### LCD Display
**Ubicación recomendada:**
- Sobre tablero (visión directa del conductor)
- Columna A (pilar lado conductor)
- Consola central (si hay espacio)
- Pod gauge estilo OEM

**Consideraciones:**
- ✅ Visible sin desviar mucho la vista del camino
- ✅ No obstruir airbags ni controles importantes
- ✅ Proteger de luz solar directa (puede lavar la pantalla)
- ✅ Usar soporte ajustable si es posible
- ⚠️ Verificar que no interfiera con palanca de cambios
- ⚠️ No tapar controles de clima/radio

### Sensor NTC Temperatura Aceite
**Opciones de montaje:**

**Opción A: Tapón del cárter**
- Reemplazar tapón de drenaje por adaptador con puerto sensor
- Ventaja: Instalación simple, mide temp real del cárter
- Desventaja: Expuesto a golpes, necesita bajar el auto

**Opción B: Línea de presión de aceite**
- Instalar T-fitting en línea existente
- Ventaja: Protegido, temp más representativa (aceite en circulación)
- Desventaja: Requiere modificar líneas

**Opción C: Cabeza de filtro**
- Adaptador tipo "sandwich" entre bloque y filtro
- Ventaja: No modifica líneas, fácil acceso
- Desventaja: Temp puede ser un poco más alta

**Instalación del sensor:**
```
1. Aplicar thread sealant apropiado (NO Teflon en sensores de temp)
2. Torque: 10-15 ft-lb típico (verificar especificaciones)
3. Conectar cable del sensor con conectores waterproof
4. Rutear cable lejos de manifold/turbo (calor extremo)
5. Aislar conexiones con heat shrink + selante
```

⚠️ **CRÍTICO**: Verificar que el cable del sensor NO roza con:
- Partes móviles del motor
- Correas/poleas
- Colector de escape/turbo

---

## 🔌 RUTEO DE CABLES

### Principios generales:
1. **Separar por tipo:**
   - Power (12V): Lo más corto posible, calibre adecuado
   - Señales (sensores): Alejados de power, protegidos
   - Serial (MS2): Cable apantallado si hay interferencia

2. **Protección mecánica:**
   - Usar loom/conduit en zonas expuestas
   - Pasar por pasa-cables de goma (firewall)
   - Evitar bordes filosos (pueden cortar cable)
   - Fijar con bridas cada 15-20cm

3. **Evitar interferencias:**
   - Cables de señal lejos de:
     * Cables de chispa (bobinas, bujías)
     * Alternador
     * Motor de arranque
     * Relays de alta corriente

### Desde batería al módulo principal:
```
Batería (+) → Fusible 2A → [Rutear por costado] → Step-down
                                                       ↓
                                                   Arduino
```
- Cable AWG 18-20 (suficiente para 2A)
- Lo más corto posible para minimizar caída de voltaje
- Evitar calor del motor

### Desde sensor NTC al Arduino:
```
Sensor NTC (motor) → [Firewall] → [Cabina] → Arduino A2
```
- Cable AWG 22-24 (señal, baja corriente)
- Si es largo (>2m), considerar cable apantallado
- Evitar ruteo paralelo con cables de chispa

### Desde MS2 al Arduino:
```
MS2 (ECU) → [Típicamente bajo tablero] → Arduino RX/TX
```
- Cable serial: AWG 24-26
- Si hay interferencia: usar cable apantallado
- Mantener lejos de cables de power

---

## ⚡ ALIMENTACIÓN ELÉCTRICA

### Punto de toma de 12V

**Opción A: ACC (Accessory)**
- Se apaga con llave
- Uso: Instalaciones que no deben drenar batería
- Típico: Fusible de radio/lighter

**Opción B: IGN (Ignition)**
- Enciende con llave en ON
- Uso: Instrumentos que necesitan estar ON antes del arranque
- Típico: Fusible de instrumentos/ECU

**Opción C: Battery Direct**
- Siempre encendido
- Uso: Si querés ver datos con motor apagado
- ⚠️ Puede drenar batería a largo plazo

**Recomendación para este proyecto: IGN**
- Arduino enciende con llave en ON
- Disponible inmediatamente al arrancar
- No drena batería cuando está apagado

### Conexión paso a paso:

1. **Identificar fusible adecuado:**
   ```
   - Consultar manual del auto (diagrama de fusibles)
   - Buscar fusible IGN con capacidad disponible (>2A libre)
   - Alternativa: agregar fusible nuevo en caja de fusibles
   ```

2. **Instalar fusible inline:**
   ```
   12V source → [Fusible 2A inline] → Step-down Input
   ```
   - Fusible tipo blade o ATO
   - Holder para fusible resistente a vibración
   - Ubicar cerca del punto de toma

3. **Ground (GND):**
   ```
   Step-down GND → [Cable corto] → Chasis limpio
   ```
   - Limpiar punto de ground (metal, no pintura)
   - Usar terminal tipo ojo con tornillo
   - Aplicar grasa dieléctrica (prevenir corrosión)

---

## 🔧 MONTAJE MECÁNICO

### Gabinete para Arduino + Step-down

**Requerimientos:**
- No conductor (plástico) o con separadores
- Ventilación (ranuras o agujeros)
- Tamaño: ~10cm x 8cm x 5cm mínimo
- Montaje: velcro industrial o tornillos

**Opciones:**
1. **Caja proyecto plástica** (tiendas electrónica)
2. **Impresión 3D** (personalizado)
3. **Caja estanca IP65** (si necesitas protección extrema)

**Montaje interno:**
```
Base de caja:
  - Arduino fijado con separadores (M3 screws + standoffs)
  - Step-down pegado o atornillado
  - Borneras para conexiones externas (opcional)
```

### Soporte para LCD

**Opciones comerciales:**
- Pod gauge 52mm universal (requiere adaptador)
- Soporte RAM mount + bracket custom
- Consola triple gauge (si pensás agregar más displays)

**DIY - Impresión 3D:**
```
Diseño básico:
  - Base con ángulo ajustable
  - Ventana para LCD
  - Montaje con adhesivo 3M VHB o tornillos
  - Paso de cables por atrás
```

**Instalación:**
1. Limpiar superficie con alcohol
2. Aplicar adhesivo o perforar para tornillos
3. Montar soporte
4. Fijar LCD
5. Conectar cable I2C (dejar holgura para vibración)

---

## 🌡️ CONSIDERACIONES TÉRMICAS

### Rango de operación:
- **Arduino:** -40°C a +85°C (chip ATmega)
- **LCD:** -10°C a +60°C (típico)
- **Step-down:** -40°C a +85°C (típico)
- **Sensores NTC:** -40°C a +150°C (típico)

### Temperatura en auto:
- **Cabina verano:** 40-60°C (cerrado al sol)
- **Motor compartment:** 80-120°C (áreas calientes)
- **Bajo tablero:** 30-40°C (ventilado)

### Recomendaciones:
1. **Arduino + LCD en cabina:** ✅ Rango OK
2. **Sensor NTC en motor:** ✅ Diseñado para eso
3. **Step-down:** Monitorear temperatura en verano
   - Si calienta mucho (>60°C): agregar disipador
4. **Cables:** Usar aislante apropiado (105°C rating min)

---

## 💧 PROTECCIÓN CONTRA AGUA/HUMEDAD

### Riesgo de humedad:
- Motor compartment: Alta (lluvia, lavado motor)
- Cabina: Baja (solo si hay filtración)
- Sensores externos: Alta (exposición directa)

### Protecciones:

**Sensor NTC:**
- Usar conectores waterproof (IP67)
- Heat shrink con adhesivo en conexiones
- Aplicar sellador en threads (no Teflon)

**Cables que atraviesan firewall:**
- Usar pasa-cables de goma (bushing)
- Aplicar sellador alrededor
- Evitar que el agua corra por el cable hacia adentro

**Módulo principal (Arduino):**
- Gabinete con ventilación pero protegido
- Silica gel dentro si hay mucha humedad
- Conformal coating en PCB (opcional, extremo)

---

## 🔊 REDUCCIÓN DE INTERFERENCIAS

### Fuentes comunes de ruido eléctrico:
- Alternador (ripple 12V)
- Bobinas de ignición (spikes)
- Relays (back-EMF)
- Motores eléctricos (bombas, ventiladores)

### Soluciones:

**Filtrado de power:**
```
12V → [Fusible] → [Capacitor 1000µF] → Step-down
```
- Capacitor electrolítico 1000µF 25V
- Ubicar cerca del input del step-down
- Ayuda a filtrar ripple del alternador

**Cable apantallado:**
- Usar para serial MS2 si hay interferencia
- Shield conectado a GND en UN solo lado

**Separación física:**
- Cables de señal lejos de cables de power
- Cruzar en ángulo recto si es inevitable
- Usar loom separado para señales

---

## 🧪 PRUEBAS ANTES DE LA INSTALACIÓN FINAL

### Bench test (en mesa, antes del auto):

1. **Test básico:**
   ```
   - Alimentar con fuente 12V de banco
   - Verificar que Arduino enciende
   - LCD muestra mensaje de boot
   - Botones funcionan
   - Sensor NTC lee temperatura ambiente
   ```

2. **Test de vibración:**
   ```
   - Golpear suavemente la mesa (simular vibración)
   - Verificar que no se resetea
   - Verificar que LCD no se desconecta
   - Reajustar conexiones si hay problemas
   ```

3. **Test de temperatura:**
   ```
   - Calentar sensor NTC (agua caliente, secador pelo)
   - Verificar que lectura sube correctamente
   - Enfriar (hielo): verificar que lectura baja
   ```

### Test en auto (motor apagado):

1. **Primera conexión:**
   ```
   - Conectar 12V del auto (CON fusible)
   - Girar llave a ACC: verificar encendido
   - Girar llave a ON: verificar que MS2 comunica
   - Apagar: verificar que se apaga correctamente
   ```

2. **Test de arranque:**
   ```
   - CON motor apagado: todo funciona
   - Dar arranque (cranking): verificar que no se resetea
   - Motor en marcha ralentí: monitorear datos
   ```

3. **Test de carga:**
   ```
   - Encender luces
   - Encender A/C
   - Encender radio
   - Verificar que voltaje baja pero sistema sigue estable
   ```

---

## ✅ CHECKLIST INSTALACIÓN FINAL

### Antes de cerrar todo:

- [ ] Todas las conexiones soldadas/crimpeadas correctamente
- [ ] Todos los cables aislados (heat shrink)
- [ ] No hay cables pelados expuestos
- [ ] Fusible correcto instalado (2A)
- [ ] Step-down ajustado a 5.0V
- [ ] Arduino montado firmemente (no se mueve)
- [ ] LCD montado y visible
- [ ] Sensor NTC instalado con torque correcto
- [ ] No hay cables rozando con partes móviles
- [ ] Ruteo limpio y profesional
- [ ] Ground limpio y firme
- [ ] Todas las conexiones verificadas con multímetro

### Test funcional final:

- [ ] Arduino enciende con llave en ON
- [ ] LCD muestra datos correctos
- [ ] Temperatura aceite lee valor real
- [ ] Presiones muestran valores correctos (motor en marcha)
- [ ] MS2 comunica correctamente
- [ ] Botones responden
- [ ] No hay resets durante cranking
- [ ] No hay ruido/interferencia en lecturas
- [ ] Sistema estable después de 15min de operación

---

## 🛠️ MANTENIMIENTO

### Chequeos periódicos (cada 3-6 meses):

1. **Inspección visual:**
   - Verificar que cables no estén rozados
   - Revisar conexiones (no sueltas)
   - Verificar que no hay corrosión
   - Limpiar polvo del gabinete

2. **Verificación eléctrica:**
   - Medir voltaje step-down (debe ser 5.0V)
   - Verificar fusible (no quemado)
   - Revisar ground (limpio, firme)

3. **Test funcional:**
   - Verificar lecturas contra instrumentos OEM
   - Verificar que alertas funcionan
   - Test de botones

### Síntomas de problemas:

| Síntoma | Posible causa | Solución |
|---------|---------------|----------|
| Arduino se resetea | Voltaje inestable | Verificar step-down, agregar capacitor |
| LCD muestra basura | Interferencia/contraste | Ajustar contraste, verificar power |
| Temp incorrecta | Sensor suelto/dañado | Verificar conexión, calibrar |
| MS2 no comunica | Cable suelto/invertido | Revisar TX/RX, GND común |
| Valores erráticos | Interferencia EMI | Separar cables, apantallar |

---

## 🎯 TIPS FINALES

1. **Toma tu tiempo:**
   - No apresures la instalación
   - Verifica cada paso antes de continuar
   - Es más rápido hacerlo bien una vez que arreglar problemas después

2. **Documentá tu instalación:**
   - Toma fotos del ruteo de cables
   - Anota dónde tomaste 12V y ground
   - Guarda esquema de conexiones para futuro troubleshooting

3. **Sé profesional:**
   - Soldaduras limpias y fuertes
   - Heat shrink en todas las conexiones expuestas
   - Ruteo limpio con bridas
   - Instalación como si fuera OEM

4. **Seguridad primero:**
   - SIEMPRE usa fusible
   - Verifica polaridad 3 veces
   - No trabajes con motor en marcha (conexiones)
   - Ten extintor cerca durante primeras pruebas

---

¡Buena suerte con la instalación! 🏁
