#include <Arduino.h>
#include <Wire.h> //LIBRERIA WIRE PARA I2C

#define DIRECCION_I2C 0x08 // PRIMERA DIRECCIÓN LIBRE
#define PIN_SDA 8  // DATA DEL I2C
#define PIN_SCL 9 //CLOCK DEL I2C
#define PIN_LED 10 // GPIO10 de la ESP32-S3

void recibirEventoI2C(int cantidadBytes) { // el int cantidadBytes indica cuantos bytes se han recibido en el buffer de recepción
  while (Wire.available() > 0) { //mientras haya datos disponibles en el buffer de recepción
    char comando = Wire.read(); //leo el primer byte del buffer de recepción

    if (comando == '1') { // Si el comando recibido es '1', enciendo el LED
      digitalWrite(PIN_LED, HIGH); // Encender LED en GPIO10
    } 
    else if (comando == '0') { // Si el comando recibido es '0', apago el LED
      digitalWrite(PIN_LED, LOW);  // Apagar LED en GPIO10
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar el pin digital GPIO10 para el LED
  pinMode(PIN_LED, OUTPUT); //SALIDA DIGITAL PARA EL LED
  digitalWrite(PIN_LED, LOW); // INICIALIZAMOS A 0V POR SI ACASO

  // Inicializar I2C en modo Esclavo pasando la dirección y los pines específicos
  Wire.begin(DIRECCION_I2C, PIN_SDA, PIN_SCL, 100000); // 100 kHz I2C
  Wire.onReceive(recibirEventoI2C); // CADA VEZ QUE RECIBA UN EVENTO I2C SE LANZA LA FUNCION recibirEventoI2C

}

void loop() {
  // El bucle queda libre; los eventos I2C atienden las interrupciones
  delay(100);
}