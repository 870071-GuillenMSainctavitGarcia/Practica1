
# Práctica 1: Sistema Embebido Múltiple y Comunicación I2C (Arduino Nano 33 BLE y ESP32-S3)

## Descripción General
Este proyecto implementa un sistema embebido distribuido enfocado en la adquisición de datos de sensores, generación y tratamiento de señales (ADC, PWM, Timers) y comunicación bidireccional sobre el bus **I2C** entre dos plataformas de desarrollo:
**Arduino Nano 33 BLE** y **ESP32-S3**.


## ¿Como probar los subapartados del proyecto?

Como cada apartado es un código distinto:

1. Si vas a probar los apartados del **1, 2, 3, 4 y6**, hay que copiarlos abre el archivo de la carpeta correspondiente y ponerlos en el main.cpp para lanzarlos (ej. `src/apartado1/apartado1.cpp.bak`).
2. Si vas a probar los **Apartados 5 y 7** abre los archivos correspondientes y pruebalos en los archivos correspondientes:
   * Carga `src/mainNano.cpp` en la Arduino Nano 33 BLE.
   * Carga `src/mainESP32.cpp` en la ESP32-S3.
3. Abre el **Monitor Serie** a **115200 baudios**.

## Estructura del Proyecto

```text
Practica1/
├── include/
│   ├── BBTimer.h          # Gestión de temporizadores y timers por hardware
│   ├── ModuloADC.h        # Configuración y lectura analógica
│   └── ModuloPWM.h        # Generación de señales PWM
├── src/
│   ├── apartado1/         # Lectura periódica del ADC (1s) mediante potenciómetro (máx 3.3V)
│   ├── apartado2/         # Muestreo ADC por interrupción de Timer hardware (cada 10s)
│   ├── apartado3/         # Generación de salida PWM a 5kHz proporcional al ADC
│   ├── apartado4/         # Protocolo de comandos por UART (ADC, ADC(x), PWM(x)) mediante String
│   ├── apartado5/         # Control básico I2C Maestro-Esclavo para conmutación de LED
│   ├── apartado6/         # Muestreo IMU (Acelerómetro, Giroscopio, Magnetómetro) cada 100ms y envío UART
│   ├── apartado7/         # Sistema completo I2C: Muestreo a demanda, envío en bloque y recepción con LED
│   ├── Blink_test/        # Verificación básica de hardware
│   ├── mainNano.cpp       # Código principal para el nodo Maestro (Arduino Nano 33 BLE)
│   └── mainESP32.cpp      # Código principal para el nodo Esclavo (ESP32-S3)
├── platformio.ini         # Entornos de compilación y monitorización de PlatformIO
└── README.md              # Documentación principal