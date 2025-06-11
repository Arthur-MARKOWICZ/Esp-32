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
          }
          temperaturas.push_back(temp);
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

    
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();

          client.println("<html><body>");
          client.println("<h1>Temperatura e Controle de LEDs</h1>");
          client.println("<p>Temperatura: " + String(temp) + " C</p>");
          client.println("<p>Umidade: " + String(umidade) + " % </p>");
          if(temp>= 20 && temp <=30 && umidade >= 60){
          client.println("<p>Condicoes estao favoraveis para formar mofo</p>");
          } else{
            client.println("<p>Condicoes desfavoraveis para formar mofo</p>");
          }
          client.println("<p>media de temperatura: " + String(media_temp) + "</p>");
          if (temp >= 20 && temp <= 30) {
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
    client.stop(); 
    Serial.println("Cliente desconectado");
  }
}
