#ifndef proxymq_h
#define proxymq_h

#include "estructuras.h"
#include <stdint.h>

int register_user(char* username, char* ip_s, char* port_s);
int unregister(char* username);
int connect(char* username);
int disconnect(char* username);
int publish(char * username_publisher, char* file_name, char* descripcion);
int delete(char * username_solicitante, char * file_name);
int listusers(char * username);
int listcontent(char * username, char * username_solicitante);
int get_file(char * username, char * remote_filename, char * local_filename, char * ip_dest, int port_dest);

int comunicacion_servidor(Peticion * peticion);
int enviar_peticion(int socket_cliente, Peticion * peticion);

#endif