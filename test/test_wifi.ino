#include <WiFi.h>
const char* ssid = "Visitantes";
const char* password = "";
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  Serial.print("conectando");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi conectado");

}

void loop() {
  // put your main code here, to run repeatedly:

}
