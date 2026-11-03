#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <Arduino.h>

// --- Credenciales (Para Wokwi usa "Wokwi-GUEST" y ""; para la placa física tu red) ---
const char* const WIFI_SSID   = "Wokwi-GUEST";
const char* const WIFI_PASS   = "";
#define BOT_TOKEN               "TU_BOT_TOKEN_TELEGRAM"
const char* const TS_SERVER   = "http://api.thingspeak.com/update";
const char* const TS_API_KEY  = "TU_THINGSPEAK_WRITE_API_KEY";

// --- Pines Asignados (Placa UTN Evaluation Board V1.0) ---
const short PIN_DHT       = 33;
const short PIN_POT       = 32;
const short PIN_LED_AZUL  = 2;
const short PIN_LED_VERDE = 23;
const short PIN_OLED_SDA  = 21;
const short PIN_OLED_SCL  = 22;

// --- Constantes de Funcionamiento ---
const float UMBRAL_DEFECTO = 0.5;
const float UMBRAL_MIN     = 1.0;
const float UMBRAL_MAX     = 1.5;

const unsigned long INTERVALO_BOT = 1000;  // 1 segundo
const unsigned long INTERVALO_TS  = 30000; // 30 segundos

#endif