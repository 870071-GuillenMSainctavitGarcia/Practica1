#include <Arduino.h>
#include <Wire.h>

#define DIRECCION_ESCLAVO 0x08
#define PIN_SDA 8
#define PIN_SCL 9
#define PIN_LED 10 // GPIO10 según la configuración del hardware

// Misma estructura de datos que en la Nano 33 BLE
struct LecturaIMU {
  float ax, ay, az; // Acelerómetro (g)
  float gx, gy, gz; // Giroscopio (dps)
  float mx, my, mz; // Magnetómetro (uT)
};

// Variable para controlar la recepción
volatile bool nuevaMuestraRecibida = false;
LecturaIMU muestraActual;

// Temporización para apagar el LED tras 1 segundo
unsigned long tiempoEncendidoLED = 0;
const unsigned long DURACION_LED = 1000; // 1000 ms = 1 segundo
bool ledEncendido = false;

// Callback que se ejecuta dentro de la interrupción I2C
void recibirEventoI2C(int cantidadBytes) {
  // Verificamos que los bytes recibidos coincidan exactamente con la estructura (36 bytes)
  if (cantidadBytes == sizeof(LecturaIMU)) {
    // Leemos el bloque de bytes directamente hacia la variable en memoria
    Wire.readBytes((uint8_t*)&muestraActual, sizeof(LecturaIMU));
    
    // Indicamos que hay datos nuevos listos para procesar en el loop
    nuevaMuestraRecibida = true;
  } else {
    // Si llegan bytes con otro tamaño, vaciamos el buffer por seguridad
    while (Wire.available() > 0) {
      Wire.read();
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Configuración del GPIO10 para el LED
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Inicialización de I2C para ESP32-S3
  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.begin(DIRECCION_ESCLAVO);
  
  // Registro de la función de recepción
  Wire.onReceive(recibirEventoI2C);

  Serial.println("ESP32-S3 Esclavo I2C listo (0x08). Esperando datos...");
}

void loop() {
  // -------------------------------------------------------------
  // 1. PROCESAR DATOS NUEVOS Y ACTIVAR LED
  // -------------------------------------------------------------
  if (nuevaMuestraRecibida) {
    nuevaMuestraRecibida = false; // Resetear bandera

    // Enceder LED en GPIO10 y registrar el momento exacto
    digitalWrite(PIN_LED, HIGH);
    ledEncendido = true;
    tiempoEncendidoLED = millis();

    // Reconstrucción e impresión por UART (Serial)
    Serial.println("\n--- [NUEVA MUESTRA RECEPCIONADA VÍA I2C] ---");
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

  // -------------------------------------------------------------
  // 2. APAGADO AUTOMÁTICO DEL LED A LOS 1000 MS (No bloqueante)
  // -------------------------------------------------------------
  if (ledEncendido && (millis() - tiempoEncendidoLED >= DURACION_LED)) {
    digitalWrite(PIN_LED, LOW);
    ledEncendido = false;
  }
}