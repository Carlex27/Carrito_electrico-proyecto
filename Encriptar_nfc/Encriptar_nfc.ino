#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte bloque = 4;  // Bloque donde se escribirá la clave

String claveEncriptada = "";  // Almacenar la clave encriptada recibida

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("Esperando clave encriptada...");
}

void loop() {
    // Leer la clave encriptada del puerto serial
    if (Serial.available()) {
        claveEncriptada = Serial.readStringUntil('\n');  // Leer la clave hasta nueva línea
        Serial.print("Clave encriptada recibida: ");
        Serial.println(claveEncriptada);
        Serial.println("Clave recibida correctamente");
    }

    // Si no hay tarjeta, salir del loop
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("Tarjeta detectada. Intentando escribir...");

    // Autenticar el bloque
    MFRC522::StatusCode status;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error de autenticación: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Convertir la clave encriptada a formato byte para escribir en la tarjeta
    byte buffer[16];
    memset(buffer, 0, 16);
    memcpy(buffer, claveEncriptada.c_str(), min(claveEncriptada.length(), 16));

    // Escribir en la tarjeta RFID
    status = rfid.MIFARE_Write(bloque, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error al escribir: ");
        Serial.println(rfid.GetStatusCodeName(status));
    } else {
        Serial.println("Clave encriptada escrita con éxito.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    Serial.println("🔄 Listo para la siguiente tarjeta...");
    delay(1000);
}
