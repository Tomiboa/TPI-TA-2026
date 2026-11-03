#include "SensorDHT.h"

SensorDHT::SensorDHT(short pin) : sensor(pin, DHT22) {}

void SensorDHT::begin() {
  sensor.begin();
}

float SensorDHT::temperatura() {
  return sensor.readTemperature();
}

float SensorDHT::humedad() {
  return sensor.readHumidity();
}

boolean SensorDHT::lecturasValidas() {
  float t = sensor.readTemperature();
  float h = sensor.readHumidity();
  return (!isnan(t) && !isnan(h));
}