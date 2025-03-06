import serial
import time
from Cryptodome.Cipher import AES
import base64

# Configuración del puerto serial (ajusta el puerto según corresponda)
SERIAL_PORT = "/dev/ttyUSB0"  # Cambia si es necesario
BAUD_RATE = 9600

# Clave AES y IV (deben ser los mismos usados para encriptar)
KEY = b'1234567890abcdef'  # 16 bytes exactos
IV = b'abcdef1234567890'   # 16 bytes para el vector de inicialización

# Clave esperada después de desencriptar
CLAVE_VALIDA = "MYSECRET12345678"

def decrypt_message(encrypted_message):
    """Desencripta el mensaje con AES-128 en modo CBC."""
    cipher = AES.new(KEY, AES.MODE_CBC, IV)
    encrypted_bytes = base64.b64decode(encrypted_message)  # Decodificar Base64
    decrypted_bytes = cipher.decrypt(encrypted_bytes)
    return decrypted_bytes.decode('utf-8').strip()  # Eliminar espacios extra

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2)
    time.sleep(2)  # Esperar conexión

    while True:
        # Leer clave encriptada del Arduino
        clave_encriptada = ser.readline().decode('utf-8').strip()

        if clave_encriptada:
            print(f"Clave encriptada recibida: {clave_encriptada}")

            try:
                # Desencriptar clave
                clave_desencriptada = decrypt_message(clave_encriptada)
                print(f"Clave desencriptada: {clave_desencriptada}")

                # Validar clave
                if clave_desencriptada == CLAVE_VALIDA:
                    ser.write(b"OK\n")  # Enviar autorización al Arduino
                    print("✅ Acceso Autorizado")
                else:
                    ser.write(b"NO\n")  # Denegar acceso
                    print("⛔ Acceso Denegado")

            except Exception as e:
                print(f"Error al desencriptar: {e}")
                ser.write(b"NO\n")  # Denegar acceso en caso de error

except Exception as e:
    print(f"Error en la comunicación serial: {e}")
