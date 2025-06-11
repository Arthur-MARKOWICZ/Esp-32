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

          if (temperaturas.size() >= 50) temperaturas.erase(temperaturas.begin());
          if (umidades.size() >= 50) umidades.erase(umidades.begin());
          temperaturas.push_back(temp);
          umidades.push_back(umidade);

          float media_temp = 0;
          for (float t : temperaturas) media_temp += t;
          media_temp /= temperaturas.size();

          if (temp >= 20 && temp <= 30) {
            digitalWrite(LED1, HIGH);
            digitalWrite(LED2, LOW);
          } else {
            digitalWrite(LED1, LOW);
            digitalWrite(LED2, HIGH);
          }

          String jsonTemps = "[";
          for (size_t i = 0; i < temperaturas.size(); i++) {
            jsonTemps += String(temperaturas[i], 2);
            if (i < temperaturas.size() - 1) jsonTemps += ",";
          }
          jsonTemps += "]";

          String jsonUmid = "[";
          for (size_t i = 0; i < umidades.size(); i++) {
            jsonUmid += String(umidades[i], 2);
            if (i < umidades.size() - 1) jsonUmid += ",";
          }
          jsonUmid += "]";

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          client.println("<!DOCTYPE html><html><head><title>Temperatura</title>");
          client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
          client.println("<style>");
          client.println("body { font-family: Arial, sans-serif; margin: 20px; background-color: #f9f9f9; color: #333; }");
          client.println("h1 { text-align: center; color: #00557f; margin-bottom: 20px; }");
          client.println("p { font-size: 16px; margin: 5px 0; }");
          client.println(".canvas-container { display: flex; flex-direction: column; align-items: center; }");
          client.println("canvas { width: 100%; max-width: 700px; height: 300px; margin-bottom: 40px; background-color: white; border: 1px solid #ccc; border-radius: 8px; box-shadow: 0 2px 6px rgba(0,0,0,0.1); }");
          client.println("a { display: inline-block; margin-top: 10px; text-decoration: none; color: white; background-color: #00557f; padding: 8px 16px; border-radius: 4px; transition: background-color 0.3s ease; }");
          client.println("a:hover { background-color: #0073aa; }");
          client.println("</style></head><body>");

          client.println("<h1>Temperatura e Controle de LEDs</h1>");
          client.println("<p>Temperatura atual: " + String(temp) + " C</p>");
          client.println("<p>Umidade atual: " + String(umidade) + " %</p>");
          if (temp >= 20 && temp <= 30 && umidade >= 60) {
            client.println("<p>Condicoes favoraveis para formacao de mofo</p>");
          } else {
            client.println("<p>Condicoes desfavoraveis para formacao de mofo</p>");
          }
          client.println("<p>Media das temperaturas (ultimas leituras): " + String(media_temp, 2) + " C</p>");
          if (temp >= 20 && temp <= 30) {
            client.println("<p>LED 1 esta ACESO</p><p>LED 2 esta APAGADO</p>");
          } else {
            client.println("<p>LED 1 esta APAGADO</p><p>LED 2 esta ACESO</p>");
          }

          client.println("<div class='canvas-container'>");
          client.println("<canvas id='tempChart'></canvas>");
          client.println("<canvas id='umidChart'></canvas>");
          client.println("</div>");

          client.println("<script>");
          client.println("const tempData = " + jsonTemps + ";");
          client.println("const umidData = " + jsonUmid + ";");
          client.println("const labels = tempData.map((_, i) => i + 1);");

          client.println("new Chart(document.getElementById('tempChart').getContext('2d'), {");
          client.println("  type: 'line',");
          client.println("  data: { labels: labels, datasets: [{ label: 'Temperatura (C)', data: tempData, borderColor: 'rgba(75,192,192,1)', backgroundColor: 'rgba(75,192,192,0.2)', fill: true, tension: 0.1 }] },");
          client.println("  options: { scales: { y: { beginAtZero: false } } }");
          client.println("});");

          client.println("new Chart(document.getElementById('umidChart').getContext('2d'), {");
          client.println("  type: 'line',");
          client.println("  data: { labels: labels, datasets: [{ label: 'Umidade (%)', data: umidData, borderColor: 'rgba(255,99,132,1)', backgroundColor: 'rgba(255,99,132,0.2)', fill: true, tension: 0.1 }] },");
          client.println("  options: { scales: { y: { beginAtZero: false } } }");
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
