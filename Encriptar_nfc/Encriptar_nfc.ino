#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte bloque = 4; // Bloque donde se escribirá la clave encriptada

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Esperando una tarjeta...");
}

void loop() {
    // Esperar hasta que se detecte una nueva tarjeta
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("Tarjeta detectada. Solicitando clave...");

    // Solicitar clave a la Raspberry Pi
    Serial.println("SOLICITAR_CLAVE");

    // Esperar respuesta con la clave encriptada
    String claveEncriptada = "";
    while (Serial.available() == 0);  // Esperar datos
    claveEncriptada = Serial.readStringUntil('\n');

    Serial.print("Clave encriptada recibida: ");
    Serial.println(claveEncriptada);

    // Autenticar el bloque para escritura
    MFRC522::StatusCode status;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Clave de fábrica

    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error de autenticación: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Convertir la clave encriptada a formato byte
    byte buffer[16];
    memset(buffer, 0, 16);
    memcpy(buffer, claveEncriptada.c_str(), min(claveEncriptada.length(), 16));

    // Escribir en la tarjeta RFID
    status = rfid.MIFARE_Write(bloque, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error al escribir: ");
        Serial.println(rfid.GetStatusCodeName(status));
    } else {
        Serial.println("✅ Clave escrita en la tarjeta.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    Serial.println("🔄 Esperando otra tarjeta...");
}
