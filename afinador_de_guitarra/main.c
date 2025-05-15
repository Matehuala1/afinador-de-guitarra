/**
 * @file main.c
 * @brief Afinador de guitarra para MSP430.
 *
 * Este programa implementa un afinador de guitarra utilizando el microcontrolador MSP430.
 * Mide la frecuencia de la señal de entrada (cuerda de guitarra), detecta la nota musical más cercana,
 * la muestra en un display y enciende LEDs indicadores para mostrar si la afinación está baja, precisa o alta.
 *
 * Funcionalidades principales:
 * - Configuración de pines y temporizador.
 * - Medición de frecuencia de la señal de entrada.
 * - Detección de la nota musical más cercana.
 * - Indicación visual de la nota y del estado de afinación mediante LEDs.
 *
 * Notas soportadas: E2 (82 Hz), A (110 Hz), D (147 Hz), G (196 Hz), B (247 Hz), E4 (333 Hz).
 *
 * @author Juan Gilberto Prado Matehuala
 * @author Jonathan Brown Moreno
 * @author Jorge Eduardo Davila Garcia
 * @date 10/05/2025
 */

#include <msp430.h> 

// Enumeracion para representar las notas musicales que se afinan
typedef enum {
    E2, // 82 Hz
    A, // 110 Hz
    D, // 147 Hz
    G, // 196 Hz
    B, // 247 Hz
    E4  // 333Hz
} TiposNotas;

// Estructura que asocia una nota con su frecuencia
typedef struct {
    TiposNotas nota;
    unsigned int frequencia;
} Notas;

// Tabla de notas de referencia
Notas tablaNotas[] = {
    {E2, 82},
    {A, 110},
    {D, 147},
    {G, 196},
    {B, 247},
    {E4, 333},
};

// Declaración de funciones
void confg_pines(void);     // Configura los pines de lectura
void confg_relog(void);     // Configura el temporizador

void mostrar_nota(TiposNotas nota);
TiposNotas detectarNota(unsigned int f);
void indicarAfinacion(unsigned int freqEntrada, Notas notaRef);

// Variables globales
float factor = 0.0004;          // Factor de conversión de cuentas a segundos (100 us)
unsigned int frecuencia = 0;      // Resultado de la frecuencia
unsigned char estado_anterior = 1; // Estado anterior del pin
unsigned int cuenta = 0;          // Contador de ciclos del timer
float periodo = 0;                // Periodo medido
unsigned char nueva_medida = 0;
volatile unsigned int flancos = 0;
volatile unsigned char ventana_lista = 0;
volatile unsigned int ventana_contador = 0; // Agrega esta variable global
volatile unsigned int debounce_counter = 0; // Global

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;     // Stop watchdog timer

    confg_pines();
    confg_relog();

    __enable_interrupt();         // Habilitar interrupciones globales

    while(1){
        if (ventana_lista) {
            ventana_lista = 0;
            frecuencia = flancos * 10; // Si la ventana es de 100 ms, multiplica por 10 para Hz
            flancos = 0;

            TiposNotas nota = detectarNota(frecuencia);
            mostrar_nota(nota);
            indicarAfinacion(frecuencia, tablaNotas[nota]);
        }
    }

    return 0;
}

void confg_pines(void){

    //pin de lectura
    P6DIR = 0x00;     // Pin P6.0 como entrada
    P6REN |= 0x01;    // Habilitar resistencia en P6.0
    P6OUT |= 0x01;    // Configurar como pull-up

    //leds display
    P2DIR = 0xFD;     // LEDs del display 2.0, 2.2 2.3 2.4 2.5 2.6 2.7   FR = 1111 1101
    P2OUT = ~0xFD;     // LEDs apagados inicialmente

    //leds indicadores
    P3DIR = 0x1F;     // LEDs indicadores   3.0 3.1 3.2 3.3 3.4
    P3OUT = ~0x1F;     // LED apagado inicialmente

}

void confg_relog(void){
    TA0CCTL0 = CCIE;
    TA0CCR0 = 60; // 100 ciclos de reloj (100 us si SMCLK = 1 MHz)
    TA0CTL = TASSEL_2 | MC_1 | ID_0 | TACLR;
}

void mostrar_nota(TiposNotas nota) {
    const unsigned char segmentos[] = {
        0xF1, // E
        0xED, // A
        0xF8, // D
        0x79, // G
        0xBC, // B
        0xF1, // E
    };
    P2OUT = (P2OUT & ~0xFD) | segmentos[nota]; // Aplicar solo a segmentos
}

// Devuelve la nota mas cercana a una frecuencia dada
TiposNotas detectarNota(unsigned int f) {
    // Calcula la diferencia absoluta entre la frecuencia medida y la frecuencia de referencia de cada nota
    unsigned int dif0 = (f > tablaNotas[0].frequencia) ? f - tablaNotas[0].frequencia : tablaNotas[0].frequencia - f; // Diferencia con E2 (82 Hz)
    unsigned int dif1 = (f > tablaNotas[1].frequencia) ? f - tablaNotas[1].frequencia : tablaNotas[1].frequencia - f; // Diferencia con A2 (110 Hz)
    unsigned int dif2 = (f > tablaNotas[2].frequencia) ? f - tablaNotas[2].frequencia : tablaNotas[2].frequencia - f; // Diferencia con D3 (147 Hz)
    unsigned int dif3 = (f > tablaNotas[3].frequencia) ? f - tablaNotas[3].frequencia : tablaNotas[3].frequencia - f; // Diferencia con G3 (196 Hz)
    unsigned int dif4 = (f > tablaNotas[4].frequencia) ? f - tablaNotas[4].frequencia : tablaNotas[4].frequencia - f; // Diferencia con B3 (247 Hz)
    unsigned int dif5 = (f > tablaNotas[5].frequencia) ? f - tablaNotas[5].frequencia : tablaNotas[5].frequencia - f; // Diferencia con E4 (333 Hz)

    // Inicializa la variable "menor" con la primera diferencia para comparar despues
    unsigned int menor = dif0;

    TiposNotas mejor = E2;

    if (dif1 < menor) { menor = dif1; mejor = A; }
    if (dif2 < menor) { menor = dif2; mejor = D; }
    if (dif3 < menor) { menor = dif3; mejor = G; }
    if (dif4 < menor) { menor = dif4; mejor = B; }
    if (dif5 < menor) { menor = dif5; mejor = E4; }

    return mejor;
}

void indicarAfinacion(unsigned int freqEntrada, Notas notaRef) {
    P3OUT &= ~0x1F;

    float margen = notaRef.frequencia * 0.06f; // 6% de la frecuencia de referencia
    float bajo = notaRef.frequencia - margen;
    float alto = notaRef.frequencia + margen;

    if (freqEntrada < bajo)
        P3OUT |= 0x01; // bemol (muy baja)
    else if (freqEntrada >= bajo && freqEntrada < notaRef.frequencia)
        P3OUT |= 0x04; // leve baja (bemol)
    else if (freqEntrada >= notaRef.frequencia && freqEntrada <= alto)
        P3OUT |= 0x08; // sostenido
    else if (freqEntrada > alto)
        P3OUT |= 0x02; // muy alta
    else
        P3OUT |= 0x10; // afinado (exacto)
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void){
    unsigned char estado_actual = (P6IN & 0x01);

    // Incrementa el contador de debounce
    if (debounce_counter < 65535) debounce_counter++;

    // Detectar flanco de subida con debounce
    if ((estado_actual == 1) && (estado_anterior == 0)) {
        if (debounce_counter > 10) { // 10*100us = 1ms mínimo entre flancos
            flancos++;
            debounce_counter = 0;
        }
    }
    estado_anterior = estado_actual;

    // Ventana de 100 ms (1000 interrupciones de 100 us)
    ventana_contador++;
    if (ventana_contador >= 1000) {
        ventana_lista = 1;
        ventana_contador = 0;
    }
}