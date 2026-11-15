#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>

class DisplayOLED {
  public:
    DisplayOLED(short pinSda, short pinScl);
    boolean begin();
    void registrarComando(String cmd);
    void mostrarEstado(String titulo, String detalle);

  private:
    short _pinSda;
    short _pinScl;
    Adafruit_SH1106G display;
    String bufferCmds[3];
    String tituloActual;
    String detalleActual;
    void renderizar();
};

#endif