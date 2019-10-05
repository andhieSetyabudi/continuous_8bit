#include "Config.h"
#include "Sensor.h"

#if DHT_USED
bool Sensor::Ambient::sleep = false;
bool Sensor::Ambient::sleepBuffer = false;

DHT Sensor::Ambient::dht(DHT_PIN, DHT_TYPE);
float Sensor::Ambient::temperature = 0.0f;
float Sensor::Ambient::humidity = 0.0f;

void Sensor::Ambient::setup(void) {
  pinMode(DHT_PIN, INPUT_PULLUP);

  dht.begin();
}

void Sensor::Ambient::loop(void) {
  if (sleepBuffer != sleep) {
    sleep = sleepBuffer;
  }
  if (sleep) return;

  float temperature;
  float humidity;
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (!isnan(temperature)) Ambient::temperature = temperature;
  if (!isnan(humidity)) Ambient::humidity = humidity;
}

bool Sensor::Ambient::isSleep(void) {
  return sleep;
}

bool Sensor::Ambient::isGoingToSleep(void) {
  return sleepBuffer & !sleep;
}

void Sensor::Ambient::setSleep(bool sleep) {
  sleepBuffer = sleep;
}
#endif
