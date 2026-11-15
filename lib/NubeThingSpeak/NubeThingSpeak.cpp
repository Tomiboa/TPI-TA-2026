#include "NubeThingSpeak.h"

NubeThingSpeak::NubeThingSpeak(unsigned long canalId, const char* apiKey) {
  _canalId = canalId;
  _apiKey = apiKey;
}

void NubeThingSpeak::begin() {
  ThingSpeak.begin(cliente);
}

// Envia temperatura (field1) y humedad (field2) al canal mediante HTTP
int NubeThingSpeak::enviar(float temperatura, float humedad) {
  if (isnan(temperatura) || isnan(humedad)) {
    Serial.println("Error en la lectura del sensor DHT");
    return TS_LECTURA_NAN;
  }

  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, humedad);
  Serial.printf("Enviando datos | Temp: %.2f C y Hum: %.2f %%\r\n", temperatura, humedad);

  int codigo = ThingSpeak.writeFields(_canalId, _apiKey);
  Serial.println("ThingSpeak: " + describirCodigo(codigo));
  return codigo;
}

String NubeThingSpeak::describirCodigo(int codigo) {
  switch (codigo) {
    case TS_ENVIO_OK:    return "Datos enviados correctamente";
    case TS_LECTURA_NAN: return "Lectura invalida del DHT22";
    case -401:           return "Envio rechazado (minimo 15 s entre envios)";
    case 400:            return "API Key o canal incorrecto";
    case -301:           return "No se pudo conectar al servidor";
    default:             return "Error de envio (codigo " + String(codigo) + ")";
  }
}
