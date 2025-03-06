#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define RELAY_PIN 7 // Conectado a IN del relay

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte bloque = 4; // Bloque donde se escribió "AUTORIZADO"
bool motorEncendido = false; // Estado del motor

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Apagar motor al inicio

    Serial.println("Acerque una tarjeta para encender/apagar el motor...");
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("Tarjeta detectada. Leyendo...");

    // Autenticar el bloque
    MFRC522::StatusCode status;
    for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Clave de fábrica

    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloque, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error de autenticación: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    // Leer datos del bloque
    byte buffer[18];
    byte tamano = sizeof(buffer);

    status = rfid.MIFARE_Read(bloque, buffer, &tamano);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("Error al leer: ");
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    Serial.print("Datos leídos: ");
    for (byte i = 0; i < 16; i++) {
        Serial.write(buffer[i]); // Imprimir como caracteres
    }
    Serial.println();

    // Si la tarjeta tiene "AUTORIZADO", alternar el estado del relay
    if (strncmp((char*)buffer, "AUTORIZADO", 10) == 0) {
        motorEncendido = !motorEncendido;
        digitalWrite(RELAY_PIN, motorEncendido ? LOW : HIGH); // LOW = encender, HIGH = apagar
        
        Serial.print("✅ Tarjeta Autorizada. Motor ");
        Serial.println(motorEncendido ? "ENCENDIDO" : "APAGADO");
    } else {
        Serial.println("⛔ Acceso Denegado.");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
