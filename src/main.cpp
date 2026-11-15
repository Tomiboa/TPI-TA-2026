// TPI - Tecnologias para la Automatizacion 4k01/26 - Grupo 13

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "Configuracion.h"
#include "SensorDHT.h"
#include "ControlPlaca.h"
#include "DisplayOLED.h"
#include "NubeThingSpeak.h"

// ---------- Objetos globales ----------
WiFiClientSecure clienteSeguro;
UniversalTelegramBot bot(BOT_TOKEN, clienteSeguro);
SensorDHT sensorClima(PIN_DHT);
ControlPlaca placa(PIN_POT, PIN_LED_VERDE, PIN_LED_AZUL);
DisplayOLED oled(PIN_OLED_SDA, PIN_OLED_SCL);
NubeThingSpeak nube(TS_CHANNEL_ID, TS_API_KEY);

unsigned long ultimoChequeoBot = 0;
unsigned long ultimoEnvioTS = 0;

// ---------- Wi-Fi ----------
void conectarWiFi() {
  Serial.print("Conectando a Wi-Fi");
  oled.mostrarEstado("Conectando WiFi", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.print("\nConectado. IP: ");
  Serial.println(WiFi.localIP());
  oled.mostrarEstado("WiFi Conectado", WiFi.localIP().toString());
}

// ---------- ThingSpeak ----------
// Lee el DHT22 y sube temperatura y humedad al canal. Devuelve el codigo de ThingSpeak.
int subirAThingSpeak() {
  ultimoEnvioTS = millis(); // Reinicia el periodo para no superar el limite de 15 s
  return nube.enviar(sensorClima.temperatura(), sensorClima.humedad());
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
    "<i>Escribí el comando como texto.</i>";
  bot.sendMessage(chatId, menu, "HTML");
}

void procesarMensaje(int i) {
  String chatId = bot.messages[i].chat_id;
  String texto  = bot.messages[i].text;
  texto.trim();

  // Separar comando y argumento (para /umbral <valor>)
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
    oled.registrarComando("/start");
    enviarMenu(chatId);
  } 
  else if (cmd == "/umbral") {
    oled.registrarComando("/umbral");
    if (placa.actualizarUmbral(arg)) {
      bot.sendMessage(chatId, "✅ Umbral actualizado a <b>" + String(placa.getUmbral(), 2) + " V</b>", "HTML");
    } else {
      bot.sendMessage(chatId, "⚠️ Fuera de rango [1.0; 1.5]V. Se asume valor original: <b>0.50 V</b>", "HTML");
    }
  }
  else if (cmd == "/estado") {
    oled.registrarComando("/estado");
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
    placa.setLedVerde(false);
    oled.registrarComando("/autoon");
    bot.sendMessage(chatId, "🔄 <b>Modo Automático ACTIVADO</b>\nLínea base: " + String(placa.getLineaBase(), 2) + " V", "HTML");
  }
  else if (cmd == "/autooff") {
    placa.setModoAuto(false);
    placa.setLedVerde(true); // Enciende LED Verde indicando modo manual activo
    oled.registrarComando("/autooff");
    bot.sendMessage(chatId, "<b>Modo Automático DESACTIVADO</b> (Manual)", "HTML");
  }
  else if (cmd == "/thingup") {
    float t = sensorClima.temperatura();
    float h = sensorClima.humedad();
    int codigo = subirAThingSpeak();
    if (codigo == TS_ENVIO_OK) {
      oled.registrarComando("/thingup");
      bot.sendMessage(chatId, "☁️ <b>Datos enviados a ThingSpeak</b>\n🌡️ Temp: <b>" + String(t, 1) + " °C</b> | 💧 Hum: <b>" + String(h, 1) + " %</b>", "HTML");
    } else {
      bot.sendMessage(chatId, "⚠️ No se pudo enviar a ThingSpeak: " + nube.describirCodigo(codigo), "HTML");
    }
  }
  else if (cmd.startsWith("/display")) {
    String subCmd = cmd.substring(8);
    subCmd.toLowerCase();

    if (subCmd == "led") {
      oled.registrarComando("/displayled");
      String estado = placa.getEstadoLedAzul() ? "Led encendido" : "Led apagado";
      oled.mostrarEstado("Estado LED:", estado);
      bot.sendMessage(chatId, "🖥️ Mostrando en OLED: <b>" + estado + "</b>", "HTML");
    }
    else if (subCmd == "pot") {
      oled.registrarComando("/displaypot");
      String det = String(placa.leerVoltajePot(), 2) + "V (Umb:" + String(placa.getUmbral(), 2) + "V)";
      oled.mostrarEstado("Potenciometro:", det);
      bot.sendMessage(chatId, "🖥️ Estado del potenciómetro mostrado en OLED.", "HTML");
    }
    else if (subCmd == "dht" || subCmd == "dht22" || subCmd == "dth22") {
      oled.registrarComando("/displaydht");
      float t = sensorClima.temperatura();
      float h = sensorClima.humedad();
      String det = "T:" + String(t, 1) + "C H:" + String(h, 1) + "%";
      oled.mostrarEstado("Sensor DHT22:", det);
      bot.sendMessage(chatId, " Estado del DHT22 mostrado en OLED.", "HTML");
    }
    else {
      // Informa el error en la pantalla OLED sin meterlo al búfer de comandos exitosos
      oled.mostrarEstado("Error Display:", "Cmd no identificado");
      bot.sendMessage(chatId, "⚠️ Comando de display no identificado. Informado en pantalla.", "HTML");
    }
  }
  else {
    bot.sendMessage(chatId, " Comando no reconocido. Mandá /start para ver el menú.", "");
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
  oled.begin();

  conectarWiFi();
  clienteSeguro.setInsecure();
  nube.begin();

  Serial.println("Bot listo. Mandale /start por Telegram.");
}

void loop() {
  placa.actualizarAutomatico();

  if (millis() - ultimoChequeoBot >= INTERVALO_BOT) {
    ultimoChequeoBot = millis();
    revisarTelegram();
  }

  // Envio periodico de telemetria a ThingSpeak
  if (millis() - ultimoEnvioTS >= INTERVALO_TS) {
    subirAThingSpeak();
  }
}