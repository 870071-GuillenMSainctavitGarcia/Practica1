#ifndef MODULO_ADC_H
#define MODULO_ADC_H

#include <Arduino.h>

class ModuloADC {
private:
  uint8_t pin;

public:
  ModuloADC(uint8_t pinLectura) : pin(pinLectura) {} // Constructor que recibe el pin de lectura del ADC

  void iniciar() {
    analogReadResolution(12); // Configuración a 12 bits (0-4095) Inicialización del ADC
  }

  int leerRaw() { // Valor Crudo del ADC (0 a 4095)
    return analogRead(pin);
  }

  float leerVoltios() { // Valor en Voltios (0 a 3.3V)
    return analogRead(pin) * (3.3f / 4095.0f);
  }

  void enviarPorUART() { // Se envía a la consola serial el valor crudo y el voltaje
    int raw = leerRaw();
    float voltios = leerVoltios();
    Serial.print("ADC: ");
    Serial.print(raw);
    Serial.print(" | Voltaje: ");
    Serial.print(voltios, 3);
    Serial.println(" V");
  }
};

#endif