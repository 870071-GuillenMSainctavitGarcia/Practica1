#include <Arduino.h>
#include "BBTimer.h"
#include "ModuloADC.h"
#include "ModuloPWM.h"

// Instanciamos los periféricos como objetos
ModuloADC adc(A0);
ModuloPWM pwm(digitalPinToPinName(D2));
BBTimer timerADC(BB_TIMER3);
// De los ficheros que he creado  creo los objetos para llamar a las funciones de dentro de los ficheros. En este caso, el ADC y el PWM. El timer lo instancio con el ID del timer que quiero usar (BB_TIMER3).
// Defino el flag que me indica cuando tengo que enviar el valor del ADC por UART  
//y el buffer donde voy a ir almacenando los caracteres que me llegan por UART.
volatile bool flagEnviarADC = false;
String bufferUART = "";

// Rutina de interrupción, se activa el flag para mandar el valor al por UART
void ISR_TimerADC() {
  flagEnviarADC = true;
}

//Función  en la que se va a aprocesar el string
void procesarComando(String cmd) {
  cmd.trim(); // Utilizado para eliminar espacios en blanco al inicio y al final del comando 

  if (cmd.equals("ADC")) { // Si el STring es igual a ADC entro en la funcion creada en Modulo ADC y lo escribo por pantalla
    adc.enviarPorUART();
  } 
  else if (cmd.startsWith("ADC(") && cmd.endsWith(")")) { // Si el String es ADC(x) entro en esta funcion
    int x = cmd.substring(4, cmd.length() - 1).toInt(); //Retiro el valor de x del String y lo convierto a int
    // Se encuentra entre el valor 4 y el valor de la longitud del string menos 1
    timerADC.timerStop();
    //Parón por seguridad, por si el timer estaba corriendo, lo paro antes de volver a configurarlo
    if (x > 0) { // Compruebo que x sea mayor que cero
      timerADC.setupTimer((uint32_t)x * 1000000, ISR_TimerADC); 
      // Lanzo la función del BBTImer, cuando llegue a x *  1000000 de micros
      // Se lanzara el ISR_TimerADC, que activa el flag y manda el valor del ADC por UART
      timerADC.timerStart(); //INIcia timer que parara en los segundos que se hayan puesto

      Serial.print("OK: Lectura cada "); Serial.print(x); Serial.println("s"); //Indica que va a leer cada dos segundos
    } else { //Si no es mayor que cero, se detiene
      Serial.println("OK: Temporizador detenido.");
    }
  } 
  else if (cmd.startsWith("PWM(") && cmd.endsWith(")")) { // Si el String es PWM(x) entro en esta funcion
    
    int x = cmd.substring(4, cmd.length() - 1).toInt(); //Cojo el int para pasarlo a nuevo Duty
    if (pwm.fijarNivel(x)) { // en el fichero ModuloPWM.h se calcula si esta dentro del rango
      Serial.print("OK: PWM ajustado a nivel "); Serial.println(x); //Indica que se ha ajustado el PWM al nivel que se ha pasado
    } else {
      Serial.println("ERROR: Rango invalido para PWM (0-9)."); //No esta en el rango
    }
  } 
  else {
    Serial.println("ERROR: Comando desconocido."); // No reconoce nada
  }
}

void setup() {
  //INICIalización de la UART y espera de inicialización
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  adc.iniciar();
  pwm.iniciar();

}

//Bucle principal, se encarga de leer la UART y de enviar el valor del ADC cuando se activa el flag
void loop() {
  while (Serial.available() > 0) { // Mientras alla un valor se leen los caracteres

    char c = (char)Serial.read(); //coge el caracter nuevo
    if (c == '\n' || c == '\r') { // SI detecta un salto de línea se procesa el buffer que haya
      if (bufferUART.length() > 0) {
        procesarComando(bufferUART); 
        bufferUART = ""; // Se vacía el buffer para la siguiente lectura
      }
    } else {
      bufferUART += c; //Se suma al caracter al buffer que ya había y se va formando el string
    }
  }

  if (flagEnviarADC) { // Si esta en true se manda el valor ADC
    flagEnviarADC = false;
    adc.enviarPorUART(); // entrara dentro de la fucnion del MOduloADC que es basicamente mandarlo por pantalla
  }
}