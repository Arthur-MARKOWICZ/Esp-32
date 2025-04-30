#include <WiFi.h>
#include "time.h"

const char* ssid = "VISITANTES";
const char* password = "";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3 * 3600;
const int   daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado com sucesso!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Falha ao obter tempo.");
    return;
  }
  Serial.println("Tempo sincronizado com sucesso!");
  printLocalTime();
}

void loop() {
  delay(10000);
  printLocalTime();
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Erro ao obter tempo.");
    return;
  }
  Serial.println(&timeinfo, "Data e hora atual: %d/%m/%Y %H:%M:%S");
}
