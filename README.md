# Afinador Digital para Guitarra con MSP430

Este proyecto implementa un **afinador digital para guitarra** usando el microcontrolador **MSP430**. Detecta la frecuencia de una señal de audio (como la de una cuerda vibrante), identifica la nota más cercana entre E2, A, D, G, B y E4, y muestra si la afinación es baja (bemol), precisa o alta (sostenido) mediante LEDs.

---

##  Funcionalidades

- Lectura de señal analógica mediante uos de Hardware e interrupciones del timer.
- Cálculo de frecuencia en tiempo real.
- Comparación con notas estándar para afinación de guitarra.
- Visualización de la nota detectada en un display.
- Indicadores LED para mostrar si la nota está afinada, baja o alta.

---

## Notas Soportadas

Las notas base utilizadas están en la afinación estándar de guitarra:

| Nota | Frecuencia (Hz) |
|------|-----------------|
| E2   | 82              |
| A    | 110             |
| D    | 147             |
| G    | 196             |
| B    | 247             |
| E4   | 333             |

---

## Arquitectura del Código

- `TiposNotas`: Enumeración de notas musicales.
- `Notas`: Estructura que vincula una nota con su frecuencia.
- `tablaNotas[]`: Tabla de referencia para afinación.
- `detectarNota(f)`: Determina la nota más cercana a una frecuencia detectada.
- `indicarAfinacion(f, notaRef)`: Muestra si la cuerda está afinada, baja o alta.
- `mostrar_nota(nota)`: Visualiza la nota actual en el display.
- `TIMER0_A0_ISR`: ISR que mide la frecuencia a través de flancos de subida.

---

## Pines Utilizados (MSP430)

| Puerto | Función                          |
|--------|----------------------------------|
| P6.0   | Entrada de señal de audio        |
| P1.0   | LED indicador de lectura         |
| P2.X   | Display de nota (7 segmentos)    |
| P3.0–P3.4 | LEDs para afinación (bajo/alto/preciso) |
| P4.0   | Salida de prueba (osciloscopio)  |

---

## Cómo Funciona

1. El microcontrolador mide el período de la señal entrante mediante interrupciones del TimerA.
2. Calcula la frecuencia basada en el tiempo entre flancos.
3. Se compara la frecuencia con la tabla de notas estándar.
4. Se indica:
   - Nota detectada (en el display).
   - Estado de afinación (mediante LEDs).

---

## Requisitos

- Microcontrolador MSP430 (probado en LaunchPad MSP430G2553).
- Fuente de señal de audio (micrófono o captador piezoeléctrico).
- LEDs y display de 7 segmentos conectados a los pines indicados (PCB diseñada especificamente para esta aplicacion).

---

## Consideraciones

- El cálculo de desviación (bemol/sostenido) usa un ±6% respecto a la frecuencia de referencia.
- El código puede mejorarse usando tipos `float` para mayor precisión en comparación de afinación.
- Asegúrate de filtrar adecuadamente la señal para evitar falsas detecciones.

---

## Licencia

Este proyecto es de uso libre con fines educativos.

---

## Autor

Juan Gilberto Prado Matehuala
Jorge Eduardo Davila Garcia
Jonathan Brown Moreno
---

