#include <WiFi.h>

const char* ssid = "";
const char* password = "";

WiFiServer server(80);

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
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
  }
}
