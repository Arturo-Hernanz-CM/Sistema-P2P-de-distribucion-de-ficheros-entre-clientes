from client import client

# Configurar los parámetros del cliente
client._server = "163.117.142.41"
client._port = 5850

# Ejecutar funciones básicas
# Registrar un cliente
result = client.register("user1")
print("REGISTER:", result)

# Conectar el cliente
result = client.connect("user1")
print("CONNECT:", result)

# Comprobar que está bien conectado
result = client.listusers()
print("LIST_USERS:", result)

# Publicar un fichero
result = client.publish("prueba.txt", "description1")
print("PUBLISH:", result)

# Comprobar la publicacion
result = client.listcontent("user1")
print("LIST_CONTENT:", result)

# Eliminar un fichero
result = client.delete("prueba.txt")
print("DELETE:", result)

# Comprobar la eliminacion
result = client.listcontent("user1")
print("LIST_CONTENT:", result)

# Desconectar el cliente
result = client.disconnect("user1")
print("DISCONNECT:", result)

# Eliminar el cliente
result = client.unregister("user1")
print("UNREGISTER:", result)