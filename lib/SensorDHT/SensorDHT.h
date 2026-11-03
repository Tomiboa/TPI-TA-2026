#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include <Arduino.h>
#include <DHT.h>

class SensorDHT {
  public:
    SensorDHT(short pin);
    void begin();
    float temperatura();
    float humedad();
    boolean lecturasValidas();

  private:
    DHT sensor;
};

#endif