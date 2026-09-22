#include <Arduino.h>
#include <Wire.h>

#define DIRECCION_ESCLAVO 0x08
#define PIN_SDA 8
#define PIN_SCL 9
#define PIN_LED 10 // GPIO10 asignado en el hardware

struct LecturaIMU {
  float ax, ay, az; // Acelerómetro (g)
  float gx, gy, gz; // Giroscopio (dps)
  float mx, my, mz; // Magnetómetro (uT)
};

volatile bool nuevaMuestraRecibida = false;
LecturaIMU muestraActual;

unsigned long tiempoEncendidoLED = 0;
const unsigned long DURACION_LED = 1000; // 1 segundo
bool ledEncendido = false;

// Callback en interrupción de recepción I2C
void recibirEventoI2C(int cantidadBytes) {
  if (cantidadBytes == sizeof(LecturaIMU)) {
    // Lectura directa del bloque de bytes a la memoria de la estructura
    Wire.readBytes((uint8_t*)&muestraActual, sizeof(LecturaIMU));
    nuevaMuestraRecibida = true;
  } else {
    // Vaciar el buffer si la cantidad de bytes no coincide
    while (Wire.available() > 0) {
      Wire.read();
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configuración de salida para el LED
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Configuración I2C específica para ESP32-S3
  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.begin(DIRECCION_ESCLAVO);
  Wire.onReceive(recibirEventoI2C);

  Serial.println("ESP32-S3 ESCLAVO LISTO (0x08). Esperando datos...");
}

void loop() {
  // 1. Procesamiento de la muestra recibida e impresión por UART
  if (nuevaMuestraRecibida) {
    nuevaMuestraRecibida = false;

    // Encender LED y marcar tiempo de inicio
    digitalWrite(PIN_LED, HIGH);
    ledEncendido = true;
    tiempoEncendidoLED = millis();

    Serial.println("\n--- [NUEVA MUESTRA RECEPCIONADA VIA I2C] ---");
    Serial.print("ACC (g)  -> X: "); Serial.print(muestraActual.ax, 2);
    Serial.print(" | Y: ");          Serial.print(muestraActual.ay, 2);
    Serial.print(" | Z: ");          Serial.println(muestraActual.az, 2);

    Serial.print("GYR (dps)-> X: "); Serial.print(muestraActual.gx, 2);
    Serial.print(" | Y: ");          Serial.print(muestraActual.gy, 2);
    Serial.print(" | Z: ");          Serial.println(muestraActual.gz, 2);

    Serial.print("MAG (uT) -> X: "); Serial.print(muestraActual.mx, 2);
    Serial.print(" | Y: ");          Serial.print(muestraActual.my, 2);
    Serial.print(" | Z: ");          Serial.println(muestraActual.mz, 2);
    Serial.println("-------------------------------------------");
  }

  // 2. Apagado automático del LED sin detener la ejecución (no bloqueante)
  if (ledEncendido && (millis() - tiempoEncendidoLED >= DURACION_LED)) {
    digitalWrite(PIN_LED, LOW);
    ledEncendido = false;
  }
}