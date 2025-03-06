import serial
import time
from Cryptodome.Cipher import AES
import base64

# Cambia esto según el puerto donde esté el Arduino
SERIAL_PORT = "/dev/ttyUSB0"  # Cambia a /dev/ttyACM0 si es necesario
BAUD_RATE = 9600

# Clave AES de 16 bytes (debe mantenerse segura y ser la misma que usará el sistema para desencriptar)
KEY = b'1234567890abcdef'  # 16 bytes exactos
IV = b'abcdef1234567890'   # 16 bytes de vector de inicialización

def encrypt_message(message):
    """Cifra un mensaje con AES-128 en modo CBC y lo codifica en Base64."""
    cipher = AES.new(KEY, AES.MODE_CBC, IV)
    message_padded = message.ljust(16)  # Asegura que el mensaje tenga 16 bytes
    encrypted_bytes = cipher.encrypt(message_padded.encode('utf-8'))
    return base64.b64encode(encrypted_bytes).decode('utf-8')  # Codifica en Base64

try:
    # Iniciar la comunicación serial con el Arduino
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    time.sleep(2)  # Esperar a que el puerto esté listo

    # Mensaje de prueba (puede cambiarse)
    clave_original = "MYSECRET12345678"  # 16 caracteres exactos
    clave_encriptada = encrypt_message(clave_original)

    # Enviar la clave encriptada al Arduino
    print(f"Clave encriptada enviada: {clave_encriptada}")
    ser.write((clave_encriptada + "\n").encode('utf-8'))  # Enviar con un salto de línea

    # Leer la respuesta del Arduino
    response = ser.readline().decode('utf-8').strip()
    print(f"Arduino respondió: {response}")

    ser.close()  # Cerrar la conexión serial

except Exception as e:
    print(f"Error en la comunicación serial: {e}")


