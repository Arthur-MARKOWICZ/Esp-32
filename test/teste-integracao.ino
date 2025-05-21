#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

#define DHTPIN 4           
#define DHTTYPE DHT22      
#define WIFI_SSID "seu_SSID"
#define WIFI_PASSWORD "sua_SENHA"

DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

void setup() {
  // Inicializa o monitor serial
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando-se ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    float h = dht.readHumidity();  // Lê a umidade
    float t = dht.readTemperature();  // Lê a temperatura em °C

    if (isnan(h) || isnan(t)) {
      request->send(500, "text/plain", "Falha ao ler o sensor DHT!");
      return;
    }

    String htmlContent = "<html><body>";
    htmlContent += "<h1>Temperatura e Umidade</h1>";
    htmlContent += "<p>Temperatura: " + String(t) + " &deg;C</p>";
    htmlContent += "<p>Umidade: " + String(h) + " %</p>";
    htmlContent += "<p><a href='/'>Atualizar</a></p>";
    htmlContent += "</body></html>";
    request->send(200, "text/html", htmlContent);
  });

  // Inicia o servidor web
  server.begin();
}

void loop() {
}
