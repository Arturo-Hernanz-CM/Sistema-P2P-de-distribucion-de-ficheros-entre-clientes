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
int connect(char* username);

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

/*
Murestra la lista de usuarios conectados al usuario que lo pide.
Éxito = 0  -> LIST_USERS OK (USER1 IP1 PORT1 \n ...)
Usuario no existe = 1 -> LIST_USERS FAIL, USER DOES NOT EXIST
Usuario no conectado = 2 -> LIST_USERS FAIL, USER NOT CONNECTED
Error = 3 -> LIST_USERS FAIL
*/
int listusers(char * username_solicitante);

/*
Muestra la lista de contenidos publicados por el usuario buscado al usuario que lo pide. 
Éxito = 0  -> LIST_CONTENT OK (file_name_1 \n ...)
Usuario no existe = 1 -> LIST_CONTENT FAIL, USER DOES NOT EXIST
Usuario no conectado = 2 -> LIST_CONTENT FAIL, USER NOT CONNECTED
No existe el usuario buscado = 3 -> LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST
Error = 4 -> LIST_CONTENT FAIL
*/
int listcontent(char * username_solicitante, char * username);

/*
Descarga el fichero de un usuario al usuario que lo pide.
Éxito = 0 -> GET_FILE OK
No existe el fichero = 1 -> GET_FILE FAIL, FILE DOES NOT EXIST
Error = 2 -> GET_FILE FAIL
*/
int getfile(char * username_solicitante, char *  username, char * remote_file_mame, char * local_file_mame);


// Funciones auxiliares

/*
Comprueba si un usuario está registrado.
Éxito = Estructura con el nodo
Error = Estructura con error = 1
*/
Exist is_registered(char * username);

/*
Comprueba si un usuario está conectado.
Éxito = Estructura con los datos de la conexion
Error = Estructura con error = 1
*/
Conexion is_connected(char * username);

#endif
