#include <Arduino.h>
#include <mbed.h>

// Pines
#define ADC_PIN A0
#define PWM_PIN D2 // Pin de salida PWM para el osciloscopio

// Configuración del objeto PWM usando la API de Mbed
mbed::PwmOut salidaPWM(digitalPinToPinName(PWM_PIN));

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configurar ADC a 12 bits (0 a 4095)
  analogReadResolution(12);

  // Fijar el periodo del PWM a 5 kHz (Periodo T = 1 / 5000Hz = 200 us)
  salidaPWM.period_us(200);

  // Inicializar el duty cycle al 0%
  salidaPWM.write(0.0f);
}

void loop() {
  // 1. Leer el ADC (0 a 4095)
  int valorADC = analogRead(ADC_PIN);

  // 2. Calcular el duty cycle en rango normalizado [0.0 a 1.0] (0% a 100%)
  float dutyCycle = (float)valorADC / 4095.0f;

  // 3. Actualizar la salida PWM
  salidaPWM.write(dutyCycle);

  // 4. Mostrar valores de control por consola
  float voltajeIn = dutyCycle * 3.3f;
  Serial.print("ADC: ");
  Serial.print(valorADC);
  Serial.print(" | Vin: ");
  Serial.print(voltajeIn, 2);
  Serial.print(" V | Duty Cycle PWM: ");
  Serial.print(dutyCycle * 100.0f, 1);
  Serial.println("%");

  delay(100); // Actualización rápida para respuesta fluida al girar el potenciómetro
}