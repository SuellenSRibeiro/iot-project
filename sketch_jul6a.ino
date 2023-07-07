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
#define TEMPERATURA_CRITICA 25.0

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

// ********* DECLARAÇÃO DE VARIÁVEIS PARA E-MAIL *********
char emailServer[] = "smtp.gmail.com";
int emailServerPort = 465;
char senderEmail[] = "seuemail@gmail.com";
char senderPassword[] = "suasenha";
char recipientEmail[] = "destinatario@gmail.com";
char emailSubject[] = "Alerta de Temperatura";
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
void enviarEmail(float temp);

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
    Serial.println("Realizando leitura de sensores...");
    leituraSensores();

    Serial.print("Temperatura atual: ");
    Serial.print(temperatura);
    Serial.println("°C");

    Serial.print("Umidade atual: ");
    Serial.print(umidade);
    Serial.println("%");

    if (temperatura >= TEMPERATURA_CRITICA) {
      enviarEmail(temperatura);
    }

    controleLeitura = millis();
  }
  delay(10);
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
}

void enviarEmail(float temp) {
  if (smtp.connect(emailServer, emailServerPort)) {
    Serial.println("Conectado ao servidor de e-mail");
    if (smtp.login(senderEmail, senderPassword)) {
      Serial.println("Autenticação bem-sucedida");

      sprintf(emailBody, "A temperatura atual é %.2f°C", temp);

      if (smtp.send(senderEmail, recipientEmail, emailSubject, emailBody)) {
        Serial.println("E-mail enviado com sucesso");
      } else {
        Serial.println("Falha ao enviar e-mail");
      }
      smtp.logout();
    } else {
      Serial.println("Falha na autenticação");
    }
    smtp.disconnect();
  } else {
    Serial.println("Falha na conexão com o servidor de e-mail");
  }
}



