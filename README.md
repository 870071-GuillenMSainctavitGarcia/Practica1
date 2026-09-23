
# Práctica 1: Sistema Embebido Múltiple y Comunicación I2C (Arduino Nano 33 BLE y ESP32-S3)

## Descripción General
Este proyecto implementa un sistema embebido distribuido enfocado en la adquisición de datos de sensores, generación y tratamiento de señales (ADC, PWM, Timers) y comunicación bidireccional sobre el bus **I2C** entre dos plataformas de desarrollo:
**Arduino Nano 33 BLE** y **ESP32-S3**.
CLAUDIA ES UNA PUTA

## ¿Como probar los subapartados del proyecto?

Como cada apartado es un código distinto:

1. Si vas a probar los apartados del **1, 2, 3, 4 y 6**, hay que copiarlos abre el archivo de la carpeta correspondiente y ponerlos en el main.cpp para lanzarlos (ej. `src/apartado1/apartado1.cpp.bak`).
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
```

## APARTADO 1
* **Procedimiento:** Muestreo periódico de la tensión entregada por un potenciómetro ($0\text{V} - 3.3\text{V}$) conectado a la entrada analógica `A0`.
* **Lógica y Funciones Clave:**

  * `analogReadResolution(12)`: Configura el ADC del microcontrolador a **12 bits** (rango dinámico de 0 a 4095) para incrementar la precisión respecto a los 10 bits nativos.
  * `analogRead(A0)`: Lee el registro del convertidor analógico-digital.
  * `Serial.print()` / `Serial.println()`: Formatean la salida mostrando tanto el valor entero cuantificado como el voltaje real transformado.
  * `delay(1000)`: Marca el periodo de muestreo fijo.


## APARTADO 2
* **Procedimiento:** Muestreo determinista del ADC cada 10 segundos utilizando un Timer por hardware e interrupciones, garantizando una ejecución no bloqueante.
* **Lógica y Funciones Clave:**

  * `BBTimer miTimer(BB_TIMER3)`: Instancia un temporizador por hardware dedicado para evitar interferir con el sistema operativo Mbed.
  * `volatile bool flagLectura`: Variable de sincronización entre la rutina de servicio a interrupción (ISR) y el hilo principal (`loop`).
  * `callbackTimer()`: Rutina ISR ultra rápida ejecutada periódicamente que únicamente activa la bandera (`flagLectura = true`).
  * `miTimer.setupTimer(10000000, callbackTimer)` / `miTimer.timerStart()`: Configura el intervalo a $10.000.000\text{ µs}$ ($10\text{ s}$) y arranca el temporizador.
  * `if (flagLectura)`: El ciclo principal procesa la conversión a voltaje y la transmisión por el puerto serie solo cuando la bandera es levantada por la ISR.

## APARTADO 3
* **Procedimiento:** Generación de una señal modulada PWM a $5\text{ kHz}$ con ciclo de trabajo (*duty cycle*) dinámico y proporcional a la lectura en tiempo real del ADC.
* **Lógica y Funciones Clave:**

  * `mbed::PwmOut salidaPWM(...)`: Instancia la salida PWM haciendo uso directo de la API nativa de Mbed OS sobre el pin `D2`.
  * `salidaPWM.period_us(200)`: Ajusta el periodo del temporizador PWM a $200\text{ µs}$, lo que fija la frecuencia de salida en exactamente $5\text{ kHz}$ ($f = \frac{1}{200\text{ µs}}$).
  * `dutyCycle = (float)valorADC / 4095.0f`: Normaliza el valor de 12 bits del ADC a una escala flotante entre `0.0f` ($0\%$) y `1.0f` ($100\%$).
  * `salidaPWM.write(dutyCycle)`: Actualiza el registro de ancho de pulso del hardware PWM con la relación calculada.
  * `delay(100)`: Establece un refresco continuo a $10\text{ Hz}$ para garantizar una respuesta fluida al variar la resistencia del potenciómetro.

## APARTADO 4
* **Procedimiento:** Parser de comandos en modo texto por la interfaz UART para consultar lecturas puntuales del ADC, programar un streaming periódico con timers o ajustar el ciclo de trabajo del PWM.
* **Lógica y Funciones Clave:**

  * `ModuloADC` / `ModuloPWM` / `BBTimer`: Arquitectura modular basada en clases orientadas a objetos para abstraer la inicialización y gestión de los periféricos.
  * `procesarComando(String cmd)`: Analizador sintáctico de la cadena recibida que interpreta tres comandos del protocolo ASCII:
    * **`ADC`**: Llama a `adc.enviarPorUART()` enviando la lectura actual de forma inmediata.
    * **`ADC(x)`**: Extrae la subcadena `x` mediante `substring()`. Si $x > 0$, reconfigura `timerADC` a $x \times 10^6\text{ µs}$ para iniciar el envío periódico; si $x = 0$, detiene el temporizador.
    * **`PWM(x)`**: Extrae el nivel deseado $x \in [0, 9]$ y llama a `pwm.fijarNivel(x)` para actualizar el *duty cycle*.
  * `Serial.available()` / `bufferUART += c`: Reconstruye de forma incremental en un búfer circular los caracteres recibidos por la UART hasta detectar un carácter de fin de línea (`\n` o `\r`).
  * `volatile bool flagEnviarADC`: Sincronización mediante *flag* no bloqueante para ejecutar la transmisión por el puerto serie únicamente fuera de la ISR.

## APARTADO 5 - Maestro (Arduino Nano 33 BLE)
* **Procedimiento:** Control remoto periódico mediante el bus I2C para alternar el estado lógico de un actuador (LED) conectado en el dispositivo esclavo.
* **Lógica y Funciones Clave:**

  * `Wire.begin()`: Inicializa el periférico I2C en modo Maestro utilizando los pines nativos hardware de la placa (`A4` para SDA y `A5` para SCL).
  * `estadoLed = !estadoLed`: Conmuta el estado de una variable booleana local cada $1\text{ s}$ (`delay(1000)`).
  * `Wire.beginTransmission(0x08)`: Abre la trama de comunicación dirigiéndose a la dirección física `0x08` del esclavo.
  * `Wire.write('1')` / `Wire.write('0')`: Empaqueta un byte de comando ASCII según el estado del indicador.
  * `Wire.endTransmission()`: Envía el paquete por el bus junto al bit de STOP finalizando la transmisión del protocolo I2C.

---

## APARTADO 5 - Esclavo (ESP32-S3)
* **Procedimiento:** Recepción e interpretación por interrupción de tramas I2C para conmutar un pin digital de salida en respuesta a las órdenes emitidas por el Maestro.
* **Lógica y Funciones Clave:**

  * `Wire.begin(0x08, PIN_SDA, PIN_SCL, 100000)`: Configura el bus I2C en modo Esclavo con dirección asignada `0x08`, asignación explícita de pines GPIO (`SDA=8`, `SCL=9`) y frecuencia del reloj a $100\text{ kHz}$.
  * `Wire.onReceive(recibirEventoI2C)`: Registra la función de callback `recibirEventoI2C` para que se ejecute automáticamente por interrupción al recibir un paquete en el bus.
  * `Wire.read()`: Extrae el byte contenido en el búfer de entrada I2C.
  * `digitalWrite(PIN_LED, HIGH/LOW)`: Cambia el nivel de tensión del pin `GPIO10` para encender o apagar el LED según el carácter recibido (`'1'` o `'0'`).  

## APARTADO 6
* **Procedimiento:** Adquisición no bloqueante y empaquetado en memoria de los 9 ejes del sensor inercial LSM9DS1 (Acelerómetro, Giroscopio y Magnetómetro) a una frecuencia de $10\text{ Hz}$ ($100\text{ ms}$), con ráfaga de volcado por la UART cada $1\text{ s}$.
* **Lógica y Funciones Clave:**

  * `struct LecturaIMU`: Definición de una estructura de datos personalizada para agrupar las 9 variables flotantes ($36\text{ bytes}$) que componen una muestra vectorial completa de la IMU.
  * `IMU.begin()`: Inicialización del chip sensor **LSM9DS1** mediante la biblioteca oficial sobre la interfaz I2C/SPI interna de la placa.
  * `IMU.accelerationAvailable()` / `readAcceleration()`, `readGyroscope()`, `readMagneticField()`: Verificación de disponibilidad de nuevos datos en los registros del sensor y extracción cuantitativa en unidades físicas ($g$, $\text{dps}$ y $\mu\text{T}$).
  * **Temporización mediante `millis()`:** Control de concurrencia mediante tareas periódicas independientes para la adquisición (`INTERVALO_MUESTREO = 100` ms) y para el volcado por puerto serie (`INTERVALO_ENVIO = 1000` ms).
  * `LecturaIMU bufferMuestras[10]`: Arreglo tipo búfer que almacena un lote continuo de 10 lecturas para procesarlas o transmitirlas en bloque, reiniciando su contador (`indiceMuestra = 0`) tras la ráfaga por UART.

## APARTADO 7 - Maestro (Arduino Nano 33 BLE)
* **Procedimiento:** Sistema distribuido con control de flujo por puerto serie para la captura en ráfaga de datos inerciales y su posterior empaquetado binario y transmisión bloque a bloque sobre el bus I2C.
* **Lógica y Funciones Clave:**

  * **Control de flujo por UART:** Aguarda la recepción de cualquier carácter por la consola (`Serial.available()`) para conmutar la variable `programaActivo` (*toggle* Start/Stop) y pausar o reanudar la adquisición.
  * `LecturaIMU bufferMuestras[5]`: Búfer dinámico que acumula 5 muestras vectoriales tomadas a intervalos de $200\text{ ms}$ ($200\text{ ms} \times 5 = 1\text{ s}$).
  * `Wire.write((uint8_t*)&bufferMuestras[i], sizeof(LecturaIMU))`: Deserialización en memoria mediante *casting* de punteros para enviar el bloque estructurado completo ($36\text{ bytes}$) por el bus I2C.
  * `delay(2)`: Micro-pausa de estabilización en el bus entre ráfagas binarias consecutivas para asegurar el correcto procesamiento del búfer del esclavo.

---

## APARTADO 7 - Esclavo (ESP32-S3)
* **Procedimiento:** Reconstrucción de la estructura binaria de 36 bytes recibida por I2C, filtrado de tramas corruptas, volcado de datos por pantalla y notificación visual con retardo no bloqueante.
* **Lógica y Funciones Clave:**

  * `Wire.readBytes((uint8_t*)&muestraActual, sizeof(LecturaIMU))`: Copia atómica directa desde el búfer de entrada I2C a la estructura en memoria al recibir exactamente $36\text{ bytes}$ (`cantidadBytes == sizeof(LecturaIMU)`).
  * **Mecanismo de Descarte:** Limpia y desestima el búfer de entrada mediante `Wire.read()` en caso de recibir un número de bytes diferente al esperado (protección ante colisiones o tramas incompletas).
  * `digitalWrite(PIN_LED, HIGH)` + `tiempoEncendidoLED = millis()`: Activa un temporizador de evento visual que enciende el LED `GPIO10` al recibir una muestra válida.
  * **Temporización por `millis()` en `loop()`:** Comprueba de forma continua la duración del destello (`DURACION_LED = 1000` ms) y apaga el LED tras transcurrir $1\text{ s}$ sin bloquear el microcontrolador.