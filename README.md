# Sensor de Temperatura e Umidade IoT

Este é um projeto fictício de um sistema IoT para monitoramento de temperatura e umidade usando um Arduino Uno WiFi e um sensor DHT11. O sistema realiza a leitura dos sensores, armazena os dados em um banco de dados MySQL na nuvem e envia alertas por e-mail quando a temperatura ultrapassa um limite crítico.

## Requisitos

- Arduino Uno WiFi
- Sensor DHT11
- Acesso à internet
- Conta em um serviço de e-mail (como o Gmail) para enviar notificações por e-mail
- Servidor MySQL na nuvem (por exemplo, AWS RDS, Google Cloud SQL)

# Sensor de Temperatura e Umidade IoT

Este é um projeto fictício de um sistema IoT para monitoramento de temperatura e umidade usando um Arduino Uno WiFi e um sensor DHT11. O sistema realiza a leitura dos sensores, armazena os dados em um banco de dados MySQL na nuvem e envia alertas por e-mail quando a temperatura ultrapassa um limite crítico.

## Requisitos

- Arduino Uno WiFi
- Sensor DHT11
- Acesso à internet
- Conta em um serviço de e-mail (como o Gmail) para enviar notificações por e-mail
- Servidor MySQL na nuvem (por exemplo, AWS RDS, Google Cloud SQL)

## Configuração

Antes de executar o projeto, é necessário configurar algumas informações sensíveis, como as credenciais da rede Wi-Fi, do banco de dados e do serviço de e-mail. Siga os passos abaixo:

1. Crie um arquivo `arduino_secrets.h` na raiz do projeto.
2. Insira as informações sensíveis no arquivo `arduino_secrets.h` utilizando o seguinte formato:

```cpp
#define SECRET_SSID "nome_da_rede_wifi"
#define SECRET_PASS "senha_da_rede_wifi"

#define SECRET_PASSDB "senha_do_banco_de_dados"

#define SECRET_EMAIL "seu_email"
#define SECRET_EMAIL_PASSWORD "senha_do_email"
#define SECRET_RECIPIENT_EMAIL "email_destinatario"
```cpp

Como Executar
Siga as etapas abaixo para executar o projeto:

Conecte o Arduino Uno WiFi ao computador.
Abra o código do projeto no Arduino IDE.
Selecione a placa "Arduino Uno WiFi" nas configurações do Arduino IDE.
Compile e faça o upload do código para o Arduino.
Abra o Monitor Serial para visualizar as leituras dos sensores e o envio de dados.
Verifique se as leituras de temperatura e umidade estão sendo enviadas para o banco de dados corretamente.
Se a temperatura ultrapassar o limite crítico definido (25°C por padrão), um e-mail de alerta será enviado para o destinatário especificado.
Contribuição
Este é um projeto fictício desenvolvido para fins de demonstração. Contribuições não são necessárias neste momento.
