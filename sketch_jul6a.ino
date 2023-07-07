// ********* INCLUSÃO DE BIBLIOTECAS *********
#include <DHT.h>
#include <WiFiNINA.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <SMTP.h>

#include "arduino_secrets.h"

// ********* DEFINIÇÕES *********
#define DHTPIN 2
#define DHTTYPE DHT11
#define TEMPERATURA_CRITICA 24.0
#define TEMPERATURA_MINIMA 20.0
#define UMIDADE_MAXIMA 60

// ********* DECLARAÇÃO DE VARIÁVEIS GLOBAIS *********
float temperatura = 0;
float umidade = 0;
unsigned long controleLeitura = 0;

// ********* DECLARAÇÃO DE VARIÁVEIS PARA WIFI *********
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// ********* DECLARAÇÃO DE VARIÁVEIS PARA MySQL *********
IPAddress server_addr(85, 10, 205, 173);
char user[] = SECRET_PASSDB;

char INSERT_SQL[] = "INSERT INTO bcitest.TemperaturaUmidade (TEMPERATURE, HUMIDITY) VALUES('%f', '%f')";
char query[128];

// ********* DECLARAÇÃO DE VARIÁVEIS PARA E-MAIL *********
char emailServer[] = "smtp.gmail.com";
int emailServerPort = 465;
char senderEmail[] = SECRET_EMAIL;
char senderPassword[] = SECRET_EMAIL_PASSWORD;
char recipientEmail[] = SECRET_RECIPIENT_EMAIL;
char emailSubject[50];
char emailBody[200];

// ********* INSTANCIANDO OBJETOS *********
WiFiClient client;
MySQL_Connection conn((Client *)&client);
DHT dht(DHTPIN, DHTTYPE);
SMTP smtp;

// ********* DECLARAÇÃO DE FUNÇÕES *********
void conectaWifi();
void enviaDados();
void leituraSensores();
void enviarEmail(float temp, float umid);

// ********* INÍCIO DO SETUP *********
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, LOW);

  while (!Serial) {
    ;
  }

  conectaWifi();

  while (!conn.connect(server_addr, 3306, user, password)) {
    Serial.println("Conexão SQL falhou.");
    conn.close();
    delay(1000);
    Serial.println("Conectando SQL novamente.");
  }
  Serial.println("Conectando ao servidor SQL.");
  digitalWrite(LED_BUILTIN, HIGH);

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(DHTPIN), leituraSensores, CHANGE);
}
// ********* FIM DO SETUP *********

// ********* INICIO DO LOOP *********
void loop() {
  if ((millis() - controleLeitura) > 5000) {
    Serial.print("Realizando leitura dos sensores: ");
    leituraSensores();
    controleLeitura = millis();
  }

  if (temperatura > TEMPERATURA_CRITICA || temperatura < TEMPERATURA_MINIMA) {
    Serial.println("Temperatura fora do intervalo crítico");
    enviarEmail(temperatura, umidade);
  }

  if (umidade > UMIDADE_MAXIMA) {
    Serial.println("Umidade acima do limite máximo");
    enviarEmail(temperatura, umidade);
  }

  delay(1000);
}
// ********* FIM DO LOOP *********

void conectaWifi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to WiFi");
}

void enviaDados() {
  sprintf(query, INSERT_SQL, temperatura, umidade);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
  Serial.println("Informações Enviadas");
}

void leituraSensores() {
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Umidade: ");
  Serial.println(umidade);
}

void enviarEmail(float temp, float umid) {
  // Verifica se a temperatura está fora do intervalo crítico
  if (temp > TEMPERATURA_CRITICA || temp < TEMPERATURA_MINIMA) {
    snprintf(emailSubject, sizeof(emailSubject), "Alerta de Temperatura - %.2f°C", temp);
    snprintf(emailBody, sizeof(emailBody), "A temperatura atual é de %.2f°C, que está fora do intervalo permitido (entre %.2f°C e %.2f°C).", temp, TEMPERATURA_MINIMA, TEMPERATURA_CRITICA);
  }
  // Verifica se a umidade está acima do limite máximo
  else if (umid > UMIDADE_MAXIMA) {
    snprintf(emailSubject, sizeof(emailSubject), "Alerta de Umidade - %.2f%%", umid);
    snprintf(emailBody, sizeof(emailBody), "A umidade atual é de %.2f%%, que está acima do limite máximo permitido de %d%%.", umid, UMIDADE_MAXIMA);
  }

  Serial.println("Enviando e-mail de alerta");
  smtp.sendEmail(emailServer, emailServerPort, senderEmail, senderPassword, recipientEmail, emailSubject, emailBody);
}


