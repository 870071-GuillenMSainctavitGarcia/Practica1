#include <Arduino.h>
#include "BBTimer.h" // LIBRERIA DE ARHCIVO DEL PROFESOR

#define POT_PIN A0

// TIMER 3 LIBRE, LO ASIGNO
BBTimer miTimer(BB_TIMER3);

// Flag LECTURA
volatile bool flagLectura = false;

// Callback que ejecuta la ISR del hardware timer
void callbackTimer() {
  flagLectura = true; // Solo levantamos la bandera (ejecución ultra rápida)
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configuración del ADC a 12 bits nativos (0 a 4095)
  analogReadResolution(12);

  // Configurar el timer: 10.000.000 us = 10 segundos
  miTimer.setupTimer(10000000, callbackTimer);
  
  // Iniciar la cuenta del timer
  miTimer.timerStart();
}

void loop() {
  // Cuando se activa la bandera de la interrupción
  if (flagLectura) {
    flagLectura = false; // Bajamos la bandera

    // Leemos el ADC y convertimos a voltaje
    int valorADC = analogRead(POT_PIN);
    float voltaje = valorADC * (3.3 / 4095.0);

    // Mostramos los datos por consola
    Serial.print("Lectura por Interrupción BBTimer (10s) -> ADC: ");
    Serial.print(valorADC);
    Serial.print(" | Voltaje: ");
    Serial.print(voltaje, 3);
    Serial.println(" V");
  }

  // El microcontrolador queda libre para hacer otras tareas sin bloqueos
}