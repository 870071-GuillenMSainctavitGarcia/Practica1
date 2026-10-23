#include <Arduino.h>

// Definimos el pin de entrada analógica ENTRADA ADC
#define ANALOG_PIN A0

void setup() {
  // Iniciar la comunicación serie, velocidad a la que se comunica platformIO
  Serial.begin(115200);

  // Esperar brevemente a que se abra la consola serie
  while (!Serial && millis() < 3000);

  // Configurar la resolución del ADC a 12 MAYOR PRECISIÓN QUE LA TÍPICA DE 10 bits para la Arduino Nano 33 BLE (0 a 4095)
  analogReadResolution(12);
}

void loop() {
  // 1. Leer el valor entero del ADC (de 0 a 4095)
  int valorADC = analogRead(ANALOG_PIN);

  // 2. Convertir el valor leído a la tensión real aplicada en el pin (0 V a 3.3 V)
  float voltaje = valorADC * (3.3 / 4095.0);

  // 3. Imprimir el resultado en la consola serie
  Serial.print("Valor ADC (12 bits): ");
  Serial.print(valorADC);
  Serial.print(" | Voltaje leido: ");
  Serial.print(voltaje, 3); // Muestra 3 decimales
  Serial.println(" V"); //println para pasar a la siguiente línea

  // 4. Esperar 1 segundo 
  delay(1000);
}