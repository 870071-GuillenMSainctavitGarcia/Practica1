#include <Arduino.h>
#include <Arduino_LSM9DS1.h>

// Estructura para almacenar un lote de lecturas de los 3 sensores
struct LecturaIMU {
  float ax, ay, az; // Acelerómetro (g)
  float gx, gy, gz; // Giroscopio (dps)
  float mx, my, mz; // Magnetómetro (uT)
};

// Arreglo para guardar 10 muestras (10 muestras x 100 ms = 1 segundo)
const int TOTAL_MUESTRAS = 10;
LecturaIMU bufferMuestras[TOTAL_MUESTRAS];
int indiceMuestra = 0;

// Variables de temporización no bloqueante
unsigned long ultimoMuestreo = 0;
unsigned long ultimoEnvioUART = 0;

const unsigned long INTERVALO_MUESTREO = 100; // 100 ms
const unsigned long INTERVALO_ENVIO    = 1000; // 1000 ms (1 segundo)

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000); // Espera de seguridad USB

  // Inicializar el sensor IMU LSM9DS1 de la Nano 33 BLE
  if (!IMU.begin()) {
    Serial.println("¡Error al inicializar la IMU LSM9DS1!");
    while (1); // Bloqueo si falla el sensor
  }

  Serial.println("IMU LSM9DS1 lista. Muestreando cada 100ms y enviando cada 1s...");
}

void loop() {
  unsigned long ahora = millis();

  // -------------------------------------------------------------
  // TAREA 1: Muestreo cada 100 ms
  // -------------------------------------------------------------
  if (ahora - ultimoMuestreo >= INTERVALO_MUESTREO) {
    ultimoMuestreo = ahora;

    float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;
    float mx = 0, my = 0, mz = 0;

    // Leer Acelerómetro si hay datos disponibles
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(ax, ay, az);
    }

    // Leer Giroscopio si hay datos disponibles
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gx, gy, gz);
    }

    // Leer Magnetómetro si hay datos disponibles
    if (IMU.magneticFieldAvailable()) {
      IMU.readMagneticField(mx, my, mz);
    }

    // Guardar la muestra en el buffer si no hemos superado el límite
    if (indiceMuestra < TOTAL_MUESTRAS) {
      bufferMuestras[indiceMuestra] = {ax, ay, az, gx, gy, gz, mx, my, mz};
      indiceMuestra++;
    }
  }

  // -------------------------------------------------------------
  // TAREA 2: Transmisión UART cada 1000 ms (1 segundo)
  // -------------------------------------------------------------
  if (ahora - ultimoEnvioUART >= INTERVALO_ENVIO) {
    ultimoEnvioUART = ahora;

    Serial.println("\n========== [REPORTE IMU - 1 SEGUNDO] ==========");
    Serial.print("Muestras recolectadas: ");
    Serial.println(indiceMuestra);

    for (int i = 0; i < indiceMuestra; i++) {
      Serial.print("M[");
      Serial.print(i + 1);
      Serial.print("] ACC(g): ");
      Serial.print(bufferMuestras[i].ax, 2); Serial.print(",");
      Serial.print(bufferMuestras[i].ay, 2); Serial.print(",");
      Serial.print(bufferMuestras[i].az, 2);

      Serial.print(" | GYR(dps): ");
      Serial.print(bufferMuestras[i].gx, 2); Serial.print(",");
      Serial.print(bufferMuestras[i].gy, 2); Serial.print(",");
      Serial.print(bufferMuestras[i].gz, 2);

      Serial.print(" | MAG(uT): ");
      Serial.print(bufferMuestras[i].mx, 2); Serial.print(",");
      Serial.print(bufferMuestras[i].my, 2); Serial.print(",");
      Serial.println(bufferMuestras[i].mz, 2);
    }
    Serial.println("===============================================");

    // Reiniciar el índice para almacenar las siguientes 10 muestras
    indiceMuestra = 0;
  }
}