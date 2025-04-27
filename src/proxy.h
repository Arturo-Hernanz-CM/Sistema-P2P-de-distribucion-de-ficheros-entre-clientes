#ifndef proxymq_h
#define proxymq_h

#include "estructuras.h"
#include <stdint.h>

int register_user(char* username, char* ip_s, char* port_s);
int unregister(char* username, char* ip_s, char* port_s);
int connect(char* username, char* ip_s, char* port_s);
int disconnect(char* username, char* ip_s, char* port_s);
int publish(char * username_publisher, char* file_name, char* descripcion, char* ip_s, char* port_s);
int delete(char * username_solicitante, char * file_name, char* ip_s, char* port_s);
int listusers(char * username, char* ip_s, char* port_s);
int listcontent(char * username, char * username_solicitante, char* ip_s, char* port_s);
int get_file(char * username, char * remote_filename, char * local_filename, char * ip_dest, int port_dest);

int comunicacion_servidor(Peticion * peticion);
int enviar_peticion(int socket_cliente, Peticion * peticion);

#endif