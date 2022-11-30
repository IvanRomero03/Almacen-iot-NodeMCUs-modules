#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <SocketIoClient.h>

#if defined(ESP32)
#define SS_PIN 5
#define RST_PIN 22
#endif

const char *ssid = "STEREN COM-8200";
const char *pass = "roborregos";
const char *HOST = "almacne-iot.us-east-1.elasticbeanstalk.com";

WiFiMulti WiFiMulti;
SocketIoClient webSocket;

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

bool activePetition = false;
String Credential = "";

const int ledAzul = 26;
const int ledRojo = 12;
const int ledVerde = 14;
const int ledAmarillo = 4;

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

    SPI.begin();
    rfid.PCD_Init();

    Serial.print(F("Reader :"));
    rfid.PCD_DumpVersionToSerial();

    webSocket.on("mensaje", msgCallback);

    webSocket.on("Peticion", createPetitionCallback);

    webSocket.emit("mensaje", "hola mundo, desde autorizacion");

    pinMode(ledAzul, OUTPUT);
    pinMode(ledRojo, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledAmarillo, OUTPUT);
    digitalWrite(ledAzul, HIGH);
    digitalWrite(ledRojo, LOW);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, LOW);
    Serial.println("Iniciando el Programa");
}

int countTimeOut = 0;

void loop()
{
    webSocket.loop();
    if (activePetition)
    {
        if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
        {
            Serial.println("Tarjeta Detectada");
            Serial.print("UID tag :");
            String content = HexToString(rfid.uid.uidByte, rfid.uid.size);
            Serial.println(content);
            if (content == Credential)
            {
                ApagarLeds();
                digitalWrite(ledVerde, HIGH);
                // DigitalWrite(ledVerde, HIGH);
                Serial.println("Tarjeta Valida");
                webSocket.emit("autorized", "true");
                countTimeOut = 0;
                activePetition = false;
                Credential = "";
            }
            else
            {
                ApagarLeds();
                digitalWrite(ledRojo, HIGH);
                badCredential();
            }
        }
        else
        {
            ApagarLeds();
            digitalWrite(ledAmarillo, HIGH);
            countTimeOut++;
            // Count 30 seconds to timeout
            // the delay is 10ms
            // DigitalWrite(ledAmarillo, HIGH);
            if (countTimeOut > 3000)
            {
                timeout();
            }
        }
    }
    else
    {
        ApagarLeds();
        digitalWrite(ledAzul, HIGH);
    }

    delay(10);
}

void ApagarLeds()
{
    digitalWrite(ledAzul, LOW);
    digitalWrite(ledRojo, LOW);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, LOW);
}

void badCredential()
{
    Serial.println("Credencial Incorrecta");
    // DigitalWrite(ledRojo, HIGH);
}

void timeout()
{
    Serial.println("Tiempo de espera agotado");
    webSocket.emit("timeout", "true");
    countTimeOut = 0;
    activePetition = false;
    Credential = "";
    // DigitalWrite(ledRojo, HIGH);
}

void msgCallback(const char *payload, size_t length)
{
    String message = payload;
    Serial.println("MENSAJE");
    Serial.printf("got message: %s\n", message);
}

void createPetitionCallback(const char *payload, size_t length)
{
    digitalWrite(ledAzul, HIGH);
    String message = payload;
    Serial.println("createPetition");
    Serial.printf("got message: %s\n", message);
    activePetition = true;
    Credential = message;
}

String HexToString(byte *buffer, byte bufferSize)
{
    String DatoHexAux = "";
    for (byte i = 0; i < bufferSize; i++)
    {
        if (buffer[i] < 0x10)
        {
            DatoHexAux = DatoHexAux + "0";
            DatoHexAux = DatoHexAux + String(buffer[i], HEX);
        }
        else
        {
            DatoHexAux = DatoHexAux + String(buffer[i], HEX);
        }
    }

    for (int i = 0; i < DatoHexAux.length(); i++)
    {
        DatoHexAux[i] = toupper(DatoHexAux[i]);
    }
    return DatoHexAux;
}
