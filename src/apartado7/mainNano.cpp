#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <Wire.h>

#define DIRECCION_ESCLAVO 0x08

// Estructura de 36 bytes (9 floats x 4 bytes)
struct LecturaIMU {
  float ax, ay, az; // Acelerómetro (g)
  float gx, gy, gz; // Giroscopio (dps)
  float mx, my, mz; // Magnetómetro (uT)
};

const int TOTAL_MUESTRAS = 5;
LecturaIMU bufferMuestras[TOTAL_MUESTRAS];
int indiceMuestra = 0;
bool programaActivo = false;

unsigned long ultimoMuestreo = 0;
unsigned long ultimoEnvioUART = 0;

const unsigned long INTERVALO_MUESTREO = 200; // 200 ms
const unsigned long INTERVALO_ENVIO    = 1000; // 1000 ms

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  // Inicializar bus I2C en modo Maestro
  Wire.begin();

  // Inicializar sensor IMU interno (LSM9DS1)
  if (!IMU.begin()) {
    Serial.println("¡Error al inicializar la IMU!");
    while (1);
  }

  
  Serial.println("NANO 33 BLE MAESTRO");
  Serial.println("Envia cualquier caracter por Serial para comenzar...");
  
}

void loop() {
  // Esperar a la orden por Monitor Serie para activar el flujo
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      Serial.read();
    }
    
    // Alternar el estado (si estaba false pasa a true, y viceversa)
    programaActivo = !programaActivo;

    if (programaActivo) {
      Serial.println("\n>>> [SISTEMA ACTIVADO] Transmitiendo datos I2C... <<<");
    } else {
      Serial.println("\n>>> [SISTEMA PAUSADO] Muestreo e I2C detenidos. <<<");
      indiceMuestra = 0; // Limpiar índice del buffer al pausar
    }
  
  }

  if (!programaActivo) {
    return; // Mantiene en espera hasta recibir la tecla
  }

  unsigned long ahora = millis();

  // 1. Muestreo cada 200 ms
  if (ahora - ultimoMuestreo >= INTERVALO_MUESTREO) {
    ultimoMuestreo = ahora;

    float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;
    float mx = 0, my = 0, mz = 0;

    if (IMU.accelerationAvailable())  IMU.readAcceleration(ax, ay, az);
    if (IMU.gyroscopeAvailable())     IMU.readGyroscope(gx, gy, gz);
    if (IMU.magneticFieldAvailable()) IMU.readMagneticField(mx, my, mz);

    if (indiceMuestra < TOTAL_MUESTRAS) {
      bufferMuestras[indiceMuestra] = {ax, ay, az, gx, gy, gz, mx, my, mz};
      indiceMuestra++;
    }
  }

  // 2. Transmisión I2C binaria cada 1 segundo
  if (ahora - ultimoEnvioUART >= INTERVALO_ENVIO) {
    ultimoEnvioUART = ahora;

    for (int i = 0; i < indiceMuestra; i++) {
      Wire.beginTransmission(DIRECCION_ESCLAVO);
      // Envío del puntero de memoria del struct (36 bytes exactos)
      Wire.write((uint8_t*)&bufferMuestras[i], sizeof(LecturaIMU));
      Wire.endTransmission();
      delay(2); // Pausa de estabilización entre paquetes
    }

    indiceMuestra = 0; // Reinicio del buffer
  }
}