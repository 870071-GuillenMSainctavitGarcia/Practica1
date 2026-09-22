#include <Arduino.h>
#include <Wire.h> //LIBRERIA WIRE PARA I2C

#define DIRECCION_ESCLAVO 0x08 // PRIMERA DIRECCIÓN LIBRE, DIRECCIÓN DEL ESCLAVO ESP32-S3
bool estadoLed = false;

void setup() {
  Serial.begin(115200); //BAUDIOS PROGRAMA

  // Inicializar I2C en modo Maestro (PINOUT A4/A5 Nano 33 BLE)
  Wire.begin();
}

void loop() {
  estadoLed = !estadoLed; // Alternar estado

  Wire.beginTransmission(DIRECCION_ESCLAVO);
  if (estadoLed) {
    Wire.write('1'); // Enviar caracter para encender
  } else {
    Wire.write('0'); // Enviar caracter para apagar
  }
  Wire.endTransmission();

  delay(1000); // Repetir cada 1 segundos
}