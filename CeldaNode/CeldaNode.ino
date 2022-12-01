#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <SocketIoClient.h>

const char *ssid = "STEREN COM-8200";
const char *pass = "roborregos";
const char *HOST = "almacne-iot.us-east-1.elasticbeanstalk.com";

WiFiMulti WiFiMulti;
SocketIoClient webSocket;

const int ledAzul = 26;
const int ledVerde = 13;
const int ledAzul2 = 4;
const int ledVerde2 = 5;

void setup()
{
    Serial.begin(115200);
    WiFiMulti.addAP(ssid, pass);
    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
    }
    Serial.println("Conected");
    // print local IP:
    Serial.println(WiFi.localIP());
    webSocket.begin(HOST);

    webSocket.on("mensaje", msgCallback);

    webSocket.on("open", openCallback);

    webSocket.emit("mensaje", "hola mundo, desde celda");

    pinMode(ledAzul, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledAzul2, OUTPUT);
    pinMode(ledVerde2, OUTPUT);
    digitalWrite(ledAzul, HIGH);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAzul2, HIGH);
    digitalWrite(ledVerde2, LOW);
    Serial.println("Iniciando el Programa");
}

void loop()
{
    webSocket.loop();
}

void msgCallback(const char *payload, size_t length)
{
    Serial.println("Mensaje recibido");
    Serial.println(payload);
}

void openCallback(const char *payload, size_t length)
{
    String message = payload;
    Serial.println("Mensaje recibido: ");
    Serial.println(message);

    if (message == "1")
    {
        openFirst();
    }
    else
    {
        openSecond();
    }
}

void openFirst()
{
    digitalWrite(ledAzul, LOW);
    digitalWrite(ledVerde, HIGH);
    // 30s delay
    delay(30000);
    digitalWrite(ledAzul, HIGH);
    digitalWrite(ledVerde, LOW);
}

void openSecond()
{
    digitalWrite(ledAzul2, LOW);
    digitalWrite(ledVerde2, HIGH);
    // 30s delay
    delay(30000);
    digitalWrite(ledAzul2, HIGH);
    digitalWrite(ledVerde2, LOW);
}
