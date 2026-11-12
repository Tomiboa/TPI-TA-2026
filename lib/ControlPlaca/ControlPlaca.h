#ifndef CONTROL_PLACA_H
#define CONTROL_PLACA_H

#include <Arduino.h>

class ControlPlaca {
  public:
    ControlPlaca(short pinPot, short pinVerde, short pinAzul);
    void begin();
    float leerVoltajePot();
    boolean actualizarUmbral(String valorTexto);
    void setModoAuto(boolean activo);
    void actualizarAutomatico();

    void setLedVerde(boolean encendido);
    void setLedAzul(boolean encendido);

    float getUmbral();
    float getLineaBase();
    boolean isModoAuto();
    boolean getEstadoLedVerde();
    boolean getEstadoLedAzul();

  private:
    short _pinPot;
    short _pinVerde;
    short _pinAzul;
    float umbral;
    float lineaBase;
    boolean modoAuto;
    boolean estadoVerde;
    boolean estadoAzul;
};

#endif