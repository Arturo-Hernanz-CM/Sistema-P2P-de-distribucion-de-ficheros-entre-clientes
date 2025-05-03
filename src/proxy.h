#ifndef proxymq_h
#define proxymq_h

#include "estructuras.h"
#include <stdint.h>

int register_user(char* username, char* ip_s, char* port_s, char * fecha_hora);
int unregister(char* username, char * fecha_hora);
int connect_user(char* username, char * fecha_hora);
int disconnect(char* username, char * fecha_hora);
int publish(char * username_publisher, char* file_name, char* descripcion, char * fecha_hora);
int delete(char * username_solicitante, char * file_name, char * fecha_hora);
int listusers(char * username, char * fecha_hora);
int listcontent(char * username, char * username_solicitante, char * fecha_hora);
int get_file(char * username, char * username_solicitante, char * remote_filename, char * local_filename, char * fecha_hora);
int quit(char * username, char * fecha_hora);

int comunicacion_servidor(Peticion * peticion, char ** ip_destino, int ** port_destino);
int enviar_peticion(int socket_cliente, Peticion * peticion);

void recibir_cliente(int * socket_cliente_descriptor);
int recibir_peticion(int socket_cliente_descriptor, Peticion * peticion_local);
void* escuchar_socket_cliente(void* arg);
int comunicacion_cliente(Peticion * peticion, char * ip_dest, int port_dest);

#endif