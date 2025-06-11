#include <WiFi.h>
#include <DHT.h>
#include <vector>

#define DHTPIN 4
#define DHTTYPE DHT22
#define LED1 19
#define LED2 23

#define WIFI_SSID "Arthur"
#define WIFI_PASSWORD "test1234"

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);
std::vector<float> temperaturas;
std::vector<float> umidades;

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando-se ao WiFi...");
  }
  Serial.println("Conectado ao WiFi!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Novo cliente conectado");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n' && currentLine.length() == 0) {

          float temp = dht.readTemperature();
          float umidade = dht.readHumidity();

          if (isnan(temp) || isnan(umidade)) {
            client.println("HTTP/1.1 500 Internal Server Error");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("Falha na leitura do sensor DHT22");
            break;
          }

          if (temperaturas.size() >= 50) {
            temperaturas.erase(temperaturas.begin());
            umidades.erase(umidades.begin());
          }
          temperaturas.push_back(temp);
          umidades.push_back(umidade);

          float media_temp = 0;
          if (!temperaturas.empty()) {
            for (float t : temperaturas) {
              media_temp += t;
            }
            media_temp /= temperaturas.size();
          }

          if (temp >= 20 && temp <= 30) {
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, LOW);
          } else {
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, HIGH);
          }

          // JSON para gráficos
          String jsonTemps = "[";
          String jsonUmids = "[";
          for (size_t i = 0; i < temperaturas.size(); i++) {
            jsonTemps += String(temperaturas[i], 2);
            jsonUmids += String(umidades[i], 2);
            if (i < temperaturas.size() - 1) {
              jsonTemps += ",";
              jsonUmids += ",";
            }
          }
          jsonTemps += "]";
          jsonUmids += "]";

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          client.println("<html><head><title>Monitoramento</title>");
          client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
          client.println("<style>");
          client.println(".grafico-container { display: flex; justify-content: space-around; }");
          client.println("canvas { max-width: 45%; height: 300px; }");
          client.println("</style>");
          client.println("</head><body>");
          client.println("<h1>Temperatura e Umidade</h1>");
          client.println("<p>Temperatura atual: " + String(temp) + " C</p>");
          client.println("<p>Umidade atual: " + String(umidade) + " %</p>");
          if (temp >= 20 && temp <= 30 && umidade >= 60) {
            client.println("<p>Condicoes favoraveis para formacao de mofo</p>");
          } else {
            client.println("<p>Condicoes desfavoraveis para formacao de mofo</p>");
          }
          client.println("<p>Media das temperaturas (ultimas leituras): " + String(media_temp, 2) + " C</p>");
          if (temp >= 20 && temp <= 30) {
            client.println("<p>LED 1 esta ACESO</p>");
            client.println("<p>LED 2 esta APAGADO</p>");
          } else {
            client.println("<p>LED 1 esta APAGADO</p>");
            client.println("<p>LED 2 esta ACESO</p>");
          }

          client.println("<div class='grafico-container'>");
          client.println("<canvas id='tempChart'></canvas>");
          client.println("<canvas id='umidChart'></canvas>");
          client.println("</div>");

          client.println("<script>");
          client.println("const tempData = " + jsonTemps + ";");
          client.println("const umidData = " + jsonUmids + ";");
          client.println("const labels = tempData.map((_, i) => i + 1);");

          client.println("new Chart(document.getElementById('tempChart').getContext('2d'), {");
          client.println("  type: 'line',");
          client.println("  data: { labels: labels, datasets: [{");
          client.println("    label: 'Temperatura (C)',");
          client.println("    data: tempData,");
          client.println("    borderColor: 'blue', backgroundColor: 'lightblue', fill: true, tension: 0.1");
          client.println("  }]},");
          client.println("  options: { scales: { y: { beginAtZero: false }}}");
          client.println("});");

          client.println("new Chart(document.getElementById('umidChart').getContext('2d'), {");
          client.println("  type: 'line',");
          client.println("  data: { labels: labels, datasets: [{");
          client.println("    label: 'Umidade (%)',");
          client.println("    data: umidData,");
          client.println("    borderColor: 'green', backgroundColor: 'lightgreen', fill: true, tension: 0.1");
          client.println("  }]},");
          client.println("  options: { scales: { y: { beginAtZero: false }}}");
          client.println("});");
          client.println("</script>");

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
    client.stop();
    Serial.println("Cliente desconectado");
  }
}
