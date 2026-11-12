// TPI - Tecnologias para la Automatizacion 4k01/26 - Grupo 13
// Paso 1: conectar el Wi-Fi y responder a /start desde Telegram.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Configuracion.h"
#include "SensorDHT.h"

// ---------- Objetos globales ----------
WiFiClientSecure clienteSeguro;
UniversalTelegramBot bot(BOT_TOKEN, clienteSeguro);
SensorDHT sensorClima(PIN_DHT);

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
    "/autooff - Desactivar modo automático\n\n"
    "☁️ <b>Nube</b>\n"
    "/thingup - Subir datos a ThingSpeak\n\n"
    "🖥️ <b>Display OLED</b>\n"
    "/displayled - Estado del LED\n"
    "/displaypot - Estado del potenciómetro\n"
    "/displaydht - Estado del DHT22\n\n"
    "✍️ <i>Escribí el comando como texto.</i>";
  bot.sendMessage(chatId, menu, "HTML");
}

void procesarMensaje(int i) {
  String chatId = bot.messages[i].chat_id;
  String texto  = bot.messages[i].text;
  texto.trim();

  // En grupos el comando puede llegar como /start@tpi_grupo13_bot
  int arroba = texto.indexOf('@');
  if (arroba > 0) texto = texto.substring(0, arroba);

  Serial.println("Mensaje de " + bot.messages[i].from_name + ": " + texto);

  if (texto == "/start") {
    enviarMenu(chatId);
  } else {
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

  sensorClima.begin();
  conectarWiFi();
  clienteSeguro.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  Serial.println("Bot listo. Mandale /start por Telegram.");
}

void loop() {
  if (millis() - ultimoChequeoBot >= INTERVALO_BOT) {
    ultimoChequeoBot = millis();
    revisarTelegram();
  }
}
