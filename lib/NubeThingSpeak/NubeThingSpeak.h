#ifndef NUBE_THINGSPEAK_H
#define NUBE_THINGSPEAK_H

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// Codigos de respuesta de ThingSpeak.writeFields()
const int TS_ENVIO_OK      = 200;
const int TS_LECTURA_NAN   = -1000; // Codigo propio: el DHT22 devolvio NaN

class NubeThingSpeak {
  public:
    NubeThingSpeak(unsigned long canalId, const char* apiKey);
    void begin();
    int enviar(float temperatura, float humedad);
    String describirCodigo(int codigo);

  private:
    WiFiClient cliente;
    unsigned long _canalId;
    const char* _apiKey;
};

#endif
