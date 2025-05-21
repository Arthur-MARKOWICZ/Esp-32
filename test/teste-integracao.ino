#include <WiFi.h>
#include <WebServer.h>  // Biblioteca para servidor web no ESP32
#include <DHT.h>

#define DHTPIN 2          // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22     // Tipo do sensor DHT22
#define LED1 5            // LED 1
#define LED2 4            // LED 2

#define WIFI_SSID "seu_SSID"
#define WIFI_PASSWORD "sua_SENHA"

DHT dht(DHTPIN, DHTTYPE);  // Cria o objeto DHT para o sensor

WebServer server(80); // Cria o servidor web na porta 80

void setup() {
  Serial.begin(115200);   // Inicializa o monitor serial
  dht.begin();            // Inicializa o sensor DHT22

  pinMode(LED1, OUTPUT);  // Configura LED1 como saída
  pinMode(LED2, OUTPUT);  // Configura LED2 como saída

  // Conecta à rede Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando-se ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Rota para a página principal (onde exibimos a temperatura)
  server.on("/", HTTP_GET, []() {
    float temp = dht.readTemperature(); // Lê a temperatura

    // Verifica se houve falha na leitura
    if (isnan(temp)) {
      server.send(500, "text/plain", "Falha na leitura do sensor DHT22");
      return;
    }

    // Controla os LEDs com base na temperatura
    if (temp >= 20 && temp <= 25) {
      digitalWrite(LED1, HIGH);  // Acende o LED1
      digitalWrite(LED2, LOW);   // Apaga o LED2
    } else {
      digitalWrite(LED1, LOW);   // Apaga o LED1
      digitalWrite(LED2, HIGH);  // Acende o LED2
    }

    // Monta o conteúdo HTML com a temperatura
    String htmlContent = "<html><body>";
    htmlContent += "<h1>Temperatura e Controle de LEDs</h1>";
    htmlContent += "<p>Temperatura: " + String(temp) + " °C</p>";

    // Exibe o estado dos LEDs
    if (temp >= 20 && temp <= 25) {
      htmlContent += "<p>LED 1 está ACESO</p>";
      htmlContent += "<p>LED 2 está APAGADO</p>";
    } else {
      htmlContent += "<p>LED 1 está APAGADO</p>";
      htmlContent += "<p>LED 2 está ACESO</p>";
    }

    htmlContent += "<p><a href='/'>Atualizar</a></p>";
    htmlContent += "</body></html>";

    server.send(200, "text/html", htmlContent);  // Envia a página HTML para o navegador
  });

  // Inicia o servidor web
  server.begin();
}

void loop() {
  server.handleClient();  // A função handleClient() cuida das requisições HTTP
}
