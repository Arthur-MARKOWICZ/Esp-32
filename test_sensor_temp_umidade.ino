#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN,DHTTYPE);
void setup() {
 Serial.begin(115200);
 dht.begin();

}

void loop() {
 delay(2000);
 float humidity = dht.readHumidity();
 float temperature = dht.readTemperature();
 if(isnan(humidity) || isnan(temperature)){
  Serial.println("Falha na leitura do DHT 22");
  return;
 }
 Serial.print("Temperature: ");
 Serial.print(temperature);
 Serial.print("humidity: ");
 Serial.print(humidity);

}
