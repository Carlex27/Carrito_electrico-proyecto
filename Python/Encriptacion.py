import serial
import time
from Cryptodome.Cipher import AES
import base64
import random
import string

# Configuración del puerto serial
SERIAL_PORT = "/dev/ttyUSB0"  # Cambia si es necesario
BAUD_RATE = 9600

# Clave AES de 16 bytes (debe ser la misma para encriptar y desencriptar)
KEY = b'1234567890abcdef'  # 16 bytes exactos
IV = b'abcdef1234567890'   # 16 bytes de vector de inicialización

def generar_clave_aleatoria():
    """Genera una clave aleatoria de 16 caracteres."""
    return ''.join(random.choices(string.ascii_letters + string.digits, k=16))

def encrypt_message(message):
    """Cifra un mensaje con AES-128 en modo CBC y lo codifica en Base64."""
    cipher = AES.new(KEY, AES.MODE_CBC, IV)
    message_padded = message.ljust(16)  # Asegurar 16 bytes
    encrypted_bytes = cipher.encrypt(message_padded.encode('utf-8'))
    return base64.b64encode(encrypted_bytes).decode('utf-8')  # Codificar en Base64

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    time.sleep(2)  # Esperar a que el puerto esté listo

    while True:
        # Esperar que Arduino solicite una clave
        solicitud = ser.readline().decode('utf-8').strip()

        if solicitud == "SOLICITAR_CLAVE":
            nueva_clave = generar_clave_aleatoria()
            clave_encriptada = encrypt_message(nueva_clave)

            print(f"Clave nueva: {nueva_clave} (encriptada: {clave_encriptada})")

            # Enviar clave encriptada al Arduino
            ser.write((clave_encriptada + "\n").encode('utf-8'))

except Exception as e:
    print(f"Error en la comunicación serial: {e}")
