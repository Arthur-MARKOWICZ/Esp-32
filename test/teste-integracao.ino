#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4          // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22     // Tipo do sensor DHT22
#define LED1 19            // LED 1
#define LED2 23          // LED 2

#define WIFI_SSID "Arthur"
#define WIFI_PASSWORD "test1234"

DHT dht(DHTPIN, DHTTYPE);  // Cria o objeto DHT para o sensor

WiFiServer server(80);  // Cria o servidor web na porta 80

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

  // Inicia o servidor
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Verifica se um cliente fez uma requisição

  if (client) {
    Serial.println("Novo cliente conectado");
    String currentLine = "";  // Armazena a linha recebida do cliente

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();  // Lê o caractere recebido do cliente
        Serial.write(c);

        // Se encontrou uma linha em branco, significa que a requisição foi recebida
        if (c == '\n' && currentLine.length() == 0) {
          // Lê a temperatura
          float temp = dht.readTemperature();
          float umidade = dht.readHumidity();

          // Verifica se houve falha na leitura
          if (isnan(temp)) {
            client.println("HTTP/1.1 500 Internal Server Error");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("Falha na leitura do sensor DHT22");
            break;
          }

          // Controla os LEDs com base na temperatura
          if (temp >= 20 && temp <= 25) {
            digitalWrite(LED1, HIGH);  // Acende o LED1
            digitalWrite(LED2, LOW);   // Apaga o LED2
          } else {
            digitalWrite(LED1, LOW);   // Apaga o LED1
            digitalWrite(LED2, HIGH);  // Acende o LED2
          }

          // Responde com um conteúdo HTML
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          client.println("<html><body>");
          client.println("<h1>Temperatura e Controle de LEDs</h1>");
          client.println("<p>Temperatura: " + String(temp) + " C</p>");
          client.println("<p>Umidade: " + String(umidade) + " % </p>");

          if (temp >= 20 && temp <= 25) {
            client.println("<p>LED 1 esta ACESO</p>");
            client.println("<p>LED 2 esta APAGADO</p>");
          } else {
            client.println("<p>LED 1 esta APAGADO</p>");
            client.println("<p>LED 2 esta ACESO</p>");
          }

          client.println("<p><a href='/'>Atualizar</a></p>");
          client.println("</body></html>");
          break;
        } else if (c == '\n') {
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();  // Fecha a conexão com o cliente
    Serial.println("Cliente desconectado");
  }
}
