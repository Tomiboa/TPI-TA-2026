#include <Arduino.h>
#include "SensorDHT.h"

// GPIO 33 según la consigna de la placa UTN
const short PIN_DHT = 33; 

SensorDHT sensorClima(PIN_DHT);

void setup() {
  Serial.begin(115200);
  delay(1000); // Pausa de estabilización inicial
  Serial.println("\n--- Iniciando prueba de SensorDHT22 ---");
  
  sensorClima.begin();
}

void loop() {
  // El DHT22 requiere al menos 2 segundos entre lecturas consecutivas
  delay(2000); 

  float temp = sensorClima.temperatura();
  float hum = sensorClima.humedad();

  if (sensorClima.lecturasValidas()) {
    Serial.printf("Temperatura: %.2f °C | Humedad: %.2f %%\n", temp, hum);
  } else {
    Serial.println("Error: No se reciben datos del sensor DHT22. Verifica conexión y VCC.");
  }
}