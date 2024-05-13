# esp8266_rs485
Conexão do ESP8266 com módulo RS485 para leitura dos registradores controladora Novus.

# Esquemático PCB

![image-20240228-183908](https://github.com/cainaalba/esp8266_rs485/assets/57020103/e3246b51-27d3-4ca6-8ad0-ccc72dae23c5)

# Componentes utilizados
1. ESP8266, com WIFI, V3;
2. Módulo leitor RS485;
3. 7 leds 3v;
4. 7 resistores 220ohms.

# PCB protótipo 1

![image-20240228-184046](https://github.com/cainaalba/esp8266_rs485/assets/57020103/cf31bafc-87fc-47c7-b9f2-d2a7ba4a9719)
Alimentação micro USB 5V.

# Funcionamento dos LEDS
**API:**
* Led VERDE: sempre ligado, API OK → Retorno 200:
* Ler VERDE e Led VERMELHO: piscando → comunicando dados com API;
* Led VERMELHO: sempre ligado → ERRO API → Retornos 400 (bad request), 500 (error).

**RS485:**
* Led VERDE: comunicação OK;
* Led VERMELHO: erro de comunicação/leitura → verificar conexões A/B, módulo RS485 e controladora.

**WIFI:**
* Led VERDE: sempre ligado → conectado na Wifi (GrupoVachileski) - por enquanto está fixo; Também é acionado o led do ESP8266 (D4 - GPIO2).
* Led VERMELHO: não conectado ou erro de conexão na Wifi (GrupoVachileski).

**TX:**
* Led AMARELO: conexão TX → recebendo dados, comunicação com RS485.

````
//Conexões dos leds, nas portas digitais do ESP8266:
const int ledRs485On = 0;  //D3 - GPIO0
const int ledWifiOn = 2;   //D4 - GPIO2
const int ledApiOn = 4;    //D2 - GPIO4

const int ledRs485Off = 12;  //D6 - GPI12
const int ledApiOff = 13;    //D7 - GPI13
const int ledWifiOff = 5;    //D1 - GPI5

const int ledDe_Re = 14;  //D5 - GPI14
````
````
//Conexões RS485 entre ESP8266:
RS485    - ESP8266
  RO     → RXD
  DE     → TXD
  DE/RE  → D5 / GPIO14
````
* Tudo é ligado em 3.3v no ESP8266. **Porta 5v não utilizada!**
* Envio de texto puro no body.
* Envia um array de 10 posições, com as informações capturadas da controladora, via RS485. Envio ocorre a cada 55 segundos (pode variar de acordo com a velocidade de comunicação com a api… pode demorar em alguns casos para ter o retorno).

# Conexão controladora NOVUS e Módulo RS485:
![image-20240229-111638](https://github.com/cainaalba/esp8266_rs485/assets/57020103/cbfb6624-e890-46c4-817e-1583495cd552)
