from client import client

# Configurar los parámetros del cliente
client._server = "127.0.0.1"
client._port = 5850

# Ejecutar funciones básicas
# Registrar un cliente
result = client.register("user1")
print("REGISTER:", result)

# Conectar el cliente
result = client.connect("user1")
print("CONNECT:", result)

# Publicar un fichero
result = client.publish("prueba.txt", "description1")
print("PUBLISH:", result)

# Comprobar la publicacion
result = client.listcontent("user1")
print("LIST_CONTENT:", result)