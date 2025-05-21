#include <WiFi.h>

IPAddress staticIP(192, 168, 1, 100); 
IPAddress gateway(192, 168, 1, 1);  
IPAddress subnet(255, 255, 0, 0);   
IPAddress dns(8, 8, 8, 8); 
const char* ssid = "VISITANTES";
const char* password = "";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP estático: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    while (client.connected() && !client.available()) {
      delay(1);
    }

    client.readStringUntil('\r');
    client.flush();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html><body><h1>teste</h1></body></html>");
    
    delay(10);
    client.stop();
  }
}
