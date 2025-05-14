#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define LED1 26
#define LED2 35

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("Falha na leitura do sensor DHT22");
    return;
  }
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" °C");
  if (temp >= 20 && temp <= 25) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  }

  delay(2000);
}
