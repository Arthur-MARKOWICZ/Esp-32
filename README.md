# Sensor OMC
## Objetivo do projeto:

  Um sistema para monitorar remotamente a temperatura e  umidade presentes em um local específico, as anotações seriam levadas para um página HTML que conteria os dados (como temperatura e umidades médias, previsões futuras, possível surgimento de mofos, etc)  e seria acessado pelo IP do ESP na rede local.


## Justificativa:

  Com o avanço da tecnologia, tornou-se possível realizar o monitoramento remoto de diversos ambientes. Isso é especialmente útil em locais onde o controle climático é essencial para garantir a qualidade, a segurança e o bom desempenho de determinados processos.

  Por exemplo, essa tecnologia pode ser extremamente útil para empresas que operam frigoríficos, ajudando a assegurar que a temperatura e a umidade dos freezers estejam dentro dos padrões adequados para um funcionamento eficiente e seguro.


## Tecnologias utilizadas:

Componentes de Hardware:

ESP32
Sensor DHT22
Resistores de pull up de 10Ω
Jumpers
Protoboard
Led RGB
Fonte de alimentação USB

Componentes de Software:

Arduino IDE (Linguagem C++)
Biblioteca DHT sensor library (by Adafruit)
Comunicação via Wi-FI/Bluetooth (WiFI.h)
Comunicação para obter o tempo real (time.h)
