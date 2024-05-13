#include <ModbusMaster.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

/*RS485  - ESP8266
  RO     → RXD
  DE     → TXD
  DE/RE  → D5 / GPIO14
  */

ModbusMaster node;

const int ledRs485On = 0;  //D3 - GPIO0
const int ledWifiOn = 2;   //D4 - GPIO2
const int ledApiOn = 4;    //D2 - GPIO4

const int ledRs485Off = 12;  //D6 - GPI12
const int ledApiOff = 13;    //D7 - GPI13
const int ledWifiOff = 5;    //D1 - GPI5

const int ledDe_Re = 14;  //D5 - GPI14

boolean conectado = false;

#define MAX485_DE ledDe_Re
#define MAX485_RE_NEG ledDe_Re

void setup() {
  inicializarPortasDigitais();

  Serial.begin(9600);

  node.begin(2, Serial);  //Slave id 2

  conectado = conectarWifi();
  delay(2000);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    String mensagem = lerRegistradores();
    comunicaApi(mensagem);
  } else {
    conectarWifi();
  }
  delay(55000);
}

void inicializarPortasDigitais() {
  pinMode(ledRs485On, OUTPUT);
  pinMode(ledWifiOn, OUTPUT);
  pinMode(ledApiOn, OUTPUT);

  pinMode(ledRs485Off, OUTPUT);
  pinMode(ledWifiOff, OUTPUT);
  pinMode(ledApiOff, OUTPUT);

  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);

  digitalWrite(ledRs485On, HIGH);  //OFF
  digitalWrite(ledWifiOn, HIGH);   //OFF
  digitalWrite(ledApiOn, HIGH);    //OFF

  digitalWrite(ledRs485Off, LOW);  //ON
  digitalWrite(ledWifiOff, LOW);   //ON
  digitalWrite(ledApiOff, LOW);    //ON
}

boolean conectarWifi() {
  const char *ssid = "SSID";
  const char *password = "SENHA";

  int cont = 1;

  WiFi.begin(ssid, password);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED && cont <= 10) {
    digitalWrite(ledWifiOff, HIGH);
    digitalWrite(ledWifiOn, LOW);
    delay(1000);
    digitalWrite(ledWifiOff, LOW);
    digitalWrite(ledWifiOn, HIGH);
    delay(1000);
    cont++;
  }

  if (WiFi.status() != WL_CONNECTED || cont == 10) {
    cont == 1;
    digitalWrite(ledWifiOn, HIGH);
    digitalWrite(ledWifiOff, LOW);
    delay(5000);
    return false;
  } else {
    // INICIA MODO DE RECEBIMENTO
    digitalWrite(MAX485_RE_NEG, 0);
    digitalWrite(MAX485_DE, 0);

    digitalWrite(ledWifiOn, LOW);
    digitalWrite(ledWifiOff, HIGH);

    node.preTransmission(preTransmission);
    delay(1000);
    node.postTransmission(postTransmission);
    delay(1000);
    return true;
  }
}

String lerRegistradores() {
  uint8_t result;

  String stringReturn = String();

  result = node.readHoldingRegisters(0x0001, 10);  //10 REGISTROS - CONTROLADOR DE TEMPERATURA NOVUS N1040
  delay(1000);

  if (result == node.ku8MBSuccess) {
    digitalWrite(ledRs485On, LOW);
    digitalWrite(ledRs485Off, HIGH);
    for (int i = 0; i < 10; i++) {
      stringReturn += String(node.getResponseBuffer(i)) + "|";
    }
    return String(stringReturn);  //node.getResponseBuffer(0) / 10.0f);
  } else {
    digitalWrite(ledRs485Off, LOW);
    digitalWrite(ledRs485On, HIGH);
    return "0";
  }
}

void comunicaApi(String mensagem) {
  const char *api = "LINK_API";
  HTTPClient http;
  WiFiClient client;

  digitalWrite(ledApiOn, LOW);
  digitalWrite(ledApiOff, LOW);

  http.begin(client, api);
  http.addHeader("Authorization", "Bearer TOKEN");
  http.addHeader("Content-Type", "text/plain");
  int codeResponse = http.POST(mensagem);
  delay(250);

  if (codeResponse != 200) {
    digitalWrite(ledApiOn, HIGH);
  } else {
    digitalWrite(ledApiOff, HIGH);
  }

  http.end();
}

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}