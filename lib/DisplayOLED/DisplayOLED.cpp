#include "DisplayOLED.h"

DisplayOLED::DisplayOLED(short pinSda, short pinScl) 
  : display(128, 64, &Wire, -1) {
  _pinSda = pinSda;
  _pinScl = pinScl;
  bufferCmds[0] = "-";
  bufferCmds[1] = "-";
  bufferCmds[2] = "-";
  tituloActual = "TPI Grupo 13";
  detalleActual = "Sistema Listo";
}

boolean DisplayOLED::begin() {
  Wire.begin(_pinSda, _pinScl);
  if (!display.begin(0x3c, true)) {
    return false;
  }
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  renderizar();
  return true;
}

// Cola FIFO: desplaza los anteriores y guarda el nuevo comando exitoso en [0]
void DisplayOLED::registrarComando(String cmd) {
  bufferCmds[2] = bufferCmds[1];
  bufferCmds[1] = bufferCmds[0];
  bufferCmds[0] = cmd;
  renderizar();
}

void DisplayOLED::mostrarEstado(String titulo, String detalle) {
  tituloActual = titulo;
  detalleActual = detalle;
  renderizar();
}

void DisplayOLED::renderizar() {
  display.clearDisplay();
  display.setCursor(0, 0);
  
  // Zona superior: Estado solicitado por /display<cmd>
  display.printf("%s\n", tituloActual.c_str());
  display.printf("%s\n", detalleActual.c_str());
  display.drawLine(0, 20, 128, 20, SH110X_WHITE);

  // Zona inferior: Búfer visual de los últimos 3 comandos exitosos
  display.setCursor(0, 24);
  display.println("Ultimos 3 cmds:");
  for (short i = 0; i < 3; i++) {
    display.printf("%d) %s\n", i + 1, bufferCmds[i].c_str());
  }
  display.display();
}