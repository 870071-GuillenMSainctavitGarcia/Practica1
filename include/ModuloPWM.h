#ifndef MODULO_PWM_H
#define MODULO_PWM_H

#include <Arduino.h>
#include <mbed.h>

class ModuloPWM {
private:
  mbed::PwmOut pwm;

public:
  ModuloPWM(PinName pinSalida) : pwm(pinSalida) {} // Constructor que recibe el pin de salida del PWM

  void iniciar() {
    pwm.period_us(200); // 5 kHz (Periodo T = 200 us)
    pwm.write(0.0f);    // Iniciar al 0%
  }

  bool fijarNivel(int nivel) { // Recibe nivel de 0 a 9
    if (nivel < 0 || nivel > 9) return false;
    float dutyCycle = (float)nivel / 9.0f; // Ciclo de trabajo del 0.0 a 1.0.
    pwm.write(dutyCycle); // La función del mbed PwmOut espera un valor entre 0.0 y 1.0 de float para convertir en Duty Cycle
    return true;
  }
};

#endif