#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <Arduino.h>

// --- Credenciales: se definen en secrets.h (NO se sube a GitHub) ---
// Copiar include/secrets_example.h como include/secrets.h y completar.
#include "secrets_example.h"

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
const unsigned long INTERVALO_TS  = 30000; // 30 segundos (ThingSpeak exige minimo 15 s)

#endif