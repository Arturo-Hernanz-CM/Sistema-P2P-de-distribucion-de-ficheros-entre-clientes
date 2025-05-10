from client import client
import time

time.sleep(2)

# Configurar los parámetros del cliente
client._server = "127.0.0.1"
client._port = 5850

# Registrar un segundo cliente
result = client.register("user2")
print("REGISTER:", result)

# Conectar un segundo cliente
result = client.connect("user2")
print("CONNECT:", result)

# Comprobar la conexion
result = client.listusers()
print("LIST_USERS:", result)

# Descargar el fichero
client.username_conectado = "user2".encode("utf-8")
result = client.getfile("user1", "prueba.txt", "prueba2.txt")
print("GETFILE:", result)