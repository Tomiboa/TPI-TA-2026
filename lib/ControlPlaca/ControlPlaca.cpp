#include "ControlPlaca.h"

ControlPlaca::ControlPlaca(short pinPot, short pinVerde, short pinAzul) {
  _pinPot = pinPot;
  _pinVerde = pinVerde;
  _pinAzul = pinAzul;
  umbral = 0.5; // Valor por defecto: 0.5V
  lineaBase = 0.0;
  modoAuto = false;
  estadoVerde = false;
  estadoAzul = false;
}

void ControlPlaca::begin() {
  pinMode(_pinVerde, OUTPUT);
  pinMode(_pinAzul, OUTPUT);
  digitalWrite(_pinVerde, LOW);
  digitalWrite(_pinAzul, LOW);
  analogReadResolution(12);
}

float ControlPlaca::leerVoltajePot() {
  return (analogRead(_pinPot) * 3.3) / 4095.0;
}

boolean ControlPlaca::actualizarUmbral(String valorTexto) {
  valorTexto.trim();
  valorTexto.replace(",", ".");

  if (valorTexto.length() > 0 && isDigit(valorTexto.charAt(0))) {
    float nuevoValor = valorTexto.toFloat();
    if (nuevoValor >= 1.0 && nuevoValor <= 1.5) {
      umbral = nuevoValor;
      return true;
    }
  }
  umbral = 0.5; // Fuera de rango [1.0; 1.5]V vuelve a 0.5V
  return false;
}

void ControlPlaca::setModoAuto(boolean activo) {
  modoAuto = activo;
  if (activo) {
    lineaBase = leerVoltajePot();
  }
}

void ControlPlaca::actualizarAutomatico() {
  if (modoAuto) {
    if (leerVoltajePot() > umbral) {
      setLedAzul(true);
    } else {
      setLedAzul(false);
    }
  }
}

void ControlPlaca::setLedVerde(boolean encendido) {
  estadoVerde = encendido;
  digitalWrite(_pinVerde, encendido ? HIGH : LOW);
}

void ControlPlaca::setLedAzul(boolean encendido) {
  estadoAzul = encendido;
  digitalWrite(_pinAzul, encendido ? HIGH : LOW);
}

float ControlPlaca::getUmbral()          { return umbral; }
float ControlPlaca::getLineaBase()       { return lineaBase; }
boolean ControlPlaca::isModoAuto()       { return modoAuto; }
boolean ControlPlaca::getEstadoLedVerde(){ return estadoVerde; }
boolean ControlPlaca::getEstadoLedAzul() { return estadoAzul; }