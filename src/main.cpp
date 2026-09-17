#include <Arduino.h>
#include <mbed.h>
#include "BBTimer.h"

#define ADC_PIN A0
#define PWM_PIN D2

// Periférico PWM (5 kHz)
mbed::PwmOut salidaPWM(digitalPinToPinName(PWM_PIN));

// Timer hardware
BBTimer timerADC(BB_TIMER3);

// Variables de control de estado
volatile bool flagEnviarADC = false;
int periodoSegundos = 0;
String comandoBuffer = "";

// Callback ejecutado por el Timer
void ISR_TimerADC() {
  flagEnviarADC = true;
}

// Función auxiliar para leer el ADC y enviar por UART
void enviarLecturaADC() {
  int valorADC = analogRead(ADC_PIN);
  float voltaje = valorADC * (3.3f / 4095.0f);
  
  Serial.print("ADC: ");
  Serial.print(valorADC);
  Serial.print(" | Voltaje: ");
  Serial.print(voltaje, 3);
  Serial.println(" V");
}

// Procesa y ejecuta las órdenes enviadas por la UART
void procesarComando(String cmd) {
  cmd.trim(); // Eliminar espacios o saltos de línea

  // Opción A: "ADC" -> Lectura puntual
  if (cmd.equals("ADC")) {
    enviarLecturaADC();
  } 
  // Opción B: "ADC(x)" -> Lectura periódica o parada
  else if (cmd.startsWith("ADC(") && cmd.endsWith(")")) {
    String valStr = cmd.substring(4, cmd.length() - 1);
    int x = valStr.toInt();

    timerADC.timerStop(); // Detener timer previo por seguridad

    if (x > 0) {
      periodoSegundos = x;
      // Convertir segundos a microsegundos (x * 1,000,000 us)
      timerADC.setupTimer((unsigned int)x * 1000000, ISR_TimerADC);
      timerADC.timerStart();
      Serial.print("Lectura de ADC configurada cada ");
      Serial.print(x);
      Serial.println(" segundos.");
    } else {
      periodoSegundos = 0;
      Serial.println("Lectura periodica de ADC desactivada.");
    }
  } 
  // Opción C: "PWM(x)" -> Ajuste del duty cycle (0 a 9)
  else if (cmd.startsWith("PWM(") && cmd.endsWith(")")) {
    String valStr = cmd.substring(4, cmd.length() - 1);
    int x = valStr.toInt();

    if (x >= 0 && x <= 9) {
      // Mapear el rango [0..9] a [0.0..1.0] (0% a 100% de Duty Cycle)
      float dutyCycle = x / 9.0f;
      salidaPWM.write(dutyCycle);

      Serial.print("PWM actualizado a nivel ");
      Serial.print(x);
      Serial.print(" (Duty Cycle: ");
      Serial.print(dutyCycle * 100.0f, 1);
      Serial.println("%)");
    } else {
      Serial.println("Error: El valor de PWM debe estar entre 0 y 9.");
    }
  } 
  else {
    Serial.println("Comando no reconocido.");
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configuración del ADC a 12 bits (0-4095)
  analogReadResolution(12);

  // Configuración del PWM a 5 kHz (200 us)
  salidaPWM.period_us(200);
  salidaPWM.write(0.0f);

  Serial.println("Sistema listo. Envia comandos: ADC, ADC(x), PWM(x)");
}

void loop() {
  // 1. Recepción y ensamblado de cadenas vía UART
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (comandoBuffer.length() > 0) {
        procesarComando(comandoBuffer);
        comandoBuffer = "";
      }
    } else {
      comandoBuffer += c;
    }
  }

  // 2. Envío de la lectura si saltó la interrupción del Timer
  if (flagEnviarADC) {
    flagEnviarADC = false;
    enviarLecturaADC();
  }
}