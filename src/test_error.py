from client import client

# Configurar los parámetros del cliente
client._server = "127.0.0.1"
client._port = 5850

# Conectar un cliente sin registrar
result = client.connect("user1")
print("CONNECT:", result)

# Eliminar un cliente no conectado
result = client.unregister("user1")
print("UNREGISTER:", result)

# Publicar un fichero
result = client.publish("prueba.txt", "description1")
print("PUBLISH:", result)



# Registrar un cliente
result = client.register("user1")
print("REGISTER:", result)

# Conectar un cliente 
result = client.connect("user1")
print("CONNECT:", result)

# Comprobar el contenido de un usuario inexistente
result = client.listcontent("user2")
print("LIST_CONTENT:", result)

# Eliminar un fichero sin publicar
result = client.delete("prueba.txt")
print("DELETE:", result)
