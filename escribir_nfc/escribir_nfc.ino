#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  // Corregido aquí

byte autorizacion[] = "AUTORIZADO"; // Datos a escribir (máximo 16 bytes)
byte bloque = 4; // Bloque donde se escribirá la autorización

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();

    Serial.println("Acerque una tarjeta para escribir la autorización...");
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("Tarjeta detectada. Intentando escribir...");

    // Autenticar el bloque
    MFRC522::StatusCode status;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Clave de fábrica

    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error de autenticación: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Asegurar que los datos ocupen 16 bytes
    byte buffer[16];
    memset(buffer, 0, 16);
    memcpy(buffer, autorizacion, min(strlen((char*)autorizacion), 16));

    // Escribir en la tarjeta
    status = rfid.MIFARE_Write(bloque, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error al escribir: ");
        Serial.println(rfid.GetStatusCodeName(status));
    } else {
        Serial.println("Autorización escrita con éxito.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();  // Detener cifrado para evitar errores con la siguiente tarjeta

    Serial.println("🔄 Listo para escribir en otra tarjeta...");
    delay(1000);
}
