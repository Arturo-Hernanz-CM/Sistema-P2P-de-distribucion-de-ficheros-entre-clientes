#include "estructuras.h"

#ifndef CLAVES_H
#define CLAVES_H

// Funciones principales

/*
Registrar un nuevo usuario en el sistema.
Éxito = 0 -> REGISTER OK
Existe el usuario = 1 -> USERNAME IN USE
Error = 2 -> REGISTER FAIL
*/
int register_user(char* nombre);

/*
Da de baja al usuario del sistema.
Éxito = 0 -> UNREGISTER OK
No existe el usuario = 1 -> USER DOES NOT EXIST
Error = 2 -> UNREGISTER FAIL
*/
int unregister(char* username);

/*
Conecta al cliente al sistema.
Éxito = 0 -> CONNECT OK
No existe el usuario = 1 -> CONNECT FAIL, USER DOES NOT EXIST
Usuario ya conectado = 2 -> USER ALREADY CONNECTED
Error = 3 -> CONNECT FAIL
*/
int connect(char* username, char* port);

/*
Desconecta al cliente del sistema.
Éxito = 0 -> DISCONNECT OK
No existe el usuario = 1 -> DISCONNECT FAIL, USER DOES NOT EXIST
Usuario no conectado = 2 -> DISCONNECT FAIL, USER NOT CONNECTED
Error = 3 -> DISCONNECT FAIL
*/
int disconnect(char * username);

/*
Publica la ruta del fichero (filename) y una breve descripción.
Éxito = 0 -> PUBLISH OK
No existe el usuario = 1 -> PUBLISH FAIL, USER DOES NOT EXIST
Usuario no conectado = 2 -> PUBLISH FAIL, USER NOT CONNECTED
Fichero ya publicado = 3 -> PUBLISH FAIL, CONTENT ALREADY PUBLISHED
Error = 4 -> PUBLISH FAIL
*/
int publish(char * username_publisher, char* file_name, char* descripcion);

/*
Elimina la publibación de un fichero.
Éxito = 0 -> DELETE OK
Usuario no existe = 1 -> DELETE FAIL, USER DOES NOT EXIST
Usuario no conectado = 2 -> DELETE FAIL, USER NOT CONNECTED
Fichero no publicado = 3 -> DELETE FAIL, CONTENT NOT PUBLISHED
Error = 4 -> DELETE FAIL
*/
int delete(char * username_solicitante, char * file_name);

// Funciones auxiliares

/*
Comprueba si un usuario está registrado.
Éxito = Estructura con el nodo
Error = Estructura con error = -1
*/
Exist is_registered(char * username);

/*
Comprueba si un usuario está conectado.
Éxito = Estructura con los datos de la conexion
Error = Estructura con error = -1
*/
Conexion is_connected(char * username);

/*
Comprueba si un fichero esta publicado.
Éxito = Estructura con la publicacion
Error = Estructura con error = -1
*/
Published is_published(char * file_name, Lista_publicaciones * publicaciones);
#endif
