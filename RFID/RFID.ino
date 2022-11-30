#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#if defined(ESP32)
  #define SS_PIN 5
  #define RST_PIN 22
#endif
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key;

byte nuidPICC[4];

String DatoHex;

void setup() 
{
   Serial.begin(115200);
   SPI.begin(); 
   rfid.PCD_Init(); 
   Serial.println();
   Serial.print(F("Reader :"));
   rfid.PCD_DumpVersionToSerial();
   //for (byte i = 0; i < 6; i++) {
   //  key.keyByte[i] = 0xFF;
   //} 
   //DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
   Serial.println();
   Serial.println();
   Serial.println("Iniciando el Programa");
}

void loop() {
  // Revisamos si hay nuevas tarjetas  presentes
  if ( rfid.PICC_IsNewCardPresent()) 
        {  
          Serial.println("Nueva tarjeta");
      //Seleccionamos una tarjeta
            if ( rfid.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print("Card UID:");
                  for (byte i = 0; i < rfid.uid.size; i++) {
                          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(rfid.uid.uidByte[i], HEX);   
                  } 
                  Serial.println();

                  String res = printHex(rfid.uid.uidByte, rfid.uid.size);
                  Serial.println("STRING: ");
                  Serial.println(res);
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  rfid.PICC_HaltA();         
            }      
  } 
}

String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}
