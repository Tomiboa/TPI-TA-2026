// TPI - Tecnologias para la Automatizacion 4k01/26 - Grupo 13
// Paso 2: Integracion de ControlPlaca (Potenciometro, Umbral, LEDs y Modo Auto).

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Configuracion.h"
#include "SensorDHT.h"
#include "ControlPlaca.h"

// ---------- Objetos globales ----------
WiFiClientSecure clienteSeguro;
UniversalTelegramBot bot(BOT_TOKEN, clienteSeguro);
SensorDHT sensorClima(PIN_DHT);
ControlPlaca placa(PIN_POT, PIN_LED_VERDE, PIN_LED_AZUL);

unsigned long ultimoChequeoBot = 0;

// ---------- Wi-Fi ----------
void conectarWiFi() {
  Serial.print("Conectando a Wi-Fi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.print("\nConectado. IP: ");
  Serial.println(WiFi.localIP());
}

// ---------- Comandos ----------
void enviarMenu(const String &chatId) {
  String menu =
    "🤖 <b>TPI Grupo 13 - ESP32</b>\n\n"
    "🌡️ <b>Consultas</b>\n"
    "/estado - Temperatura, humedad, potenciómetro y LEDs\n\n"
    "⚙️ <b>Control</b>\n"
    "/umbral &lt;valor&gt; - Fijar umbral (ej: /umbral 1.2)\n"
    "/autoon - Activar modo automático\n"
    "/autooff - Desactivar modo automático\n"
    "/verdeon | /verdeoff - Control manual LED Verde\n"
    "/azulon | /azuloff - Control manual LED Azul\n\n"
    "✍️ <i>Escribí el comando como texto.</i>";
  bot.sendMessage(chatId, menu, "HTML");
}

void procesarMensaje(int i) {
  String chatId = bot.messages[i].chat_id;
  String texto  = bot.messages[i].text;
  texto.trim();

  // Separar comando de argumento (ej: "/umbral 1.2")
  String cmd = texto;
  String arg = "";
  int espacio = texto.indexOf(' ');
  if (espacio > 0) {
    cmd = texto.substring(0, espacio);
    arg = texto.substring(espacio + 1);
    arg.trim();
  }
  int arroba = cmd.indexOf('@');
  if (arroba > 0) cmd = cmd.substring(0, arroba);

  Serial.println("Mensaje de " + bot.messages[i].from_name + ": " + texto);

  if (cmd == "/start") {
    enviarMenu(chatId);
  } 
  else if (cmd == "/umbral") {
    if (placa.actualizarUmbral(arg)) {
      bot.sendMessage(chatId, "✅ Umbral actualizado a <b>" + String(placa.getUmbral(), 2) + " V</b>", "HTML");
    } else {
      bot.sendMessage(chatId, "⚠️ Fuera de rango [1.0; 1.5]V. Se asume valor por defecto: <b>0.50 V</b>", "HTML");
    }
  }
  else if (cmd == "/estado") {
    float t = sensorClima.temperatura();
    float h = sensorClima.humedad();
    float v = placa.leerVoltajePot();

    String msg = "📊 <b>Estado del Sistema</b>\n\n";
    msg += "🌡️ Temp: <b>" + String(t, 1) + " °C</b> | 💧 Hum: <b>" + String(h, 1) + " %</b>\n";
    msg += "🎛️ Potenciómetro: <b>" + String(v, 2) + " V</b>\n";
    msg += "🎯 Umbral: <b>" + String(placa.getUmbral(), 2) + " V</b>\n";
    msg += "⚙️ Modo: <b>" + String(placa.isModoAuto() ? "Automático" : "Manual") + "</b>\n";
    msg += "🟢 LED Verde: <b>" + String(placa.getEstadoLedVerde() ? "Encendido" : "Apagado") + "</b>\n";
    msg += "🔵 LED Azul: <b>" + String(placa.getEstadoLedAzul() ? "Encendido" : "Apagado") + "</b>";
    bot.sendMessage(chatId, msg, "HTML");
  }
  else if (cmd == "/autoon") {
    placa.setModoAuto(true);
    bot.sendMessage(chatId, "🔄 <b>Modo Automático ACTIVADO</b>\nLínea base: " + String(placa.getLineaBase(), 2) + " V", "HTML");
  }
  else if (cmd == "/autooff") {
    placa.setModoAuto(false);
    bot.sendMessage(chatId, "🖐️ <b>Modo Automático DESACTIVADO</b> (Manual)", "HTML");
  }
  else if (cmd == "/verdeon" || cmd == "/verdeoff" || cmd == "/azulon" || cmd == "/azuloff") {
    if (placa.isModoAuto() && (cmd == "/azulon" || cmd == "/azuloff")) {
      bot.sendMessage(chatId, "⚠️ El LED Azul está en modo automático. Mandá /autooff primero.", "HTML");
    } else {
      if (cmd == "/verdeon")  placa.setLedVerde(true);
      if (cmd == "/verdeoff") placa.setLedVerde(false);
      if (cmd == "/azulon")   placa.setLedAzul(true);
      if (cmd == "/azuloff")  placa.setLedAzul(false);
      bot.sendMessage(chatId, "✅ LED actualizado.", "HTML");
    }
  }
  else {
    bot.sendMessage(chatId, "❓ Comando no reconocido. Mandá /start para ver el menú.", "");
  }
}

void revisarTelegram() {
  int nuevos = bot.getUpdates(bot.last_message_received + 1);
  while (nuevos) {
    for (int i = 0; i < nuevos; i++) procesarMensaje(i);
    nuevos = bot.getUpdates(bot.last_message_received + 1);
  }
}

// ---------- Programa principal ----------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- TPI Grupo 13 ---");

  placa.begin();
  sensorClima.begin();
  conectarWiFi();
  clienteSeguro.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  Serial.println("Bot listo. Mandale /start por Telegram.");
}

void loop() {
  placa.actualizarAutomatico();

  if (millis() - ultimoChequeoBot >= INTERVALO_BOT) {
    ultimoChequeoBot = millis();
    revisarTelegram();
  }
}