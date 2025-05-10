import subprocess
import time

def run_test(description, command):
    print(f"\n>>> {description}")
    process = subprocess.run(command, shell=True, capture_output=True, text=True)
    print(process.stdout)
    if process.stderr:
        print("ERRORS:\n", process.stderr)

# Lanzar el servicio web en background
print("Launching WS...")
ws = subprocess.Popen(["python3", "ws-fechahora-service.py"])
time.sleep(1)  # Espera a que el WS arranque

# Ejecutar tests individuales
run_test("Registrar el usuario 1", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'REGISTER user1'")
run_test("Conectar el usuario 1", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'CONNECT user1'")
run_test("Comprobar la lista de usuarios", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'LIST_USERS'")
run_test("Publicar el fichero prueba.txt", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'PUBLISH prueba.txt a'")
run_test("Comprobar la lista de contenidos", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'LIST_CONTENTS user1'")
run_test("Borrar el fichero 1", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'DELETE prueba1.txt'")
run_test("Desconectar el usuario", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'UNREGISTER user1'")
run_test("Eliminar el usuario", "python3 client.py -s 127.0.0.1 -p 5850 <<< 'LIST_USERS'")

# Parar el servicio web
print("Stopping WS...")
ws.terminate()