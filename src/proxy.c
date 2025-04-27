#include "proxy.h"
#include "lines.h"
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>


// Variable global
Peticion peticion = {0};
char *ip_servidor;
char *puerto_servidor_char;


// Funciones

// Enviar peticion
int enviar_peticion(int socket_cliente_descriptor, Peticion * peticion){
   int32_t key_, N_value2_, operacion_, value3_x_, value3_y_;
   uint64_t v_value2_[32];

   key_ = htonl(peticion->key);
   N_value2_ = htonl(peticion->N_value2);
   operacion_ = htonl(peticion->operacion);
   value3_x_ = htonl(peticion->value3.x);
   value3_y_ = htonl(peticion->value3.y);

    for (int i = 0; i < peticion->N_value2; i++) {
        v_value2_[i] = serializar_double(peticion->value2[i]);
    }

   // Enviar key
    if (sendMessage(socket_cliente_descriptor, (char *) &key_, sizeof(key_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar la clave\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar N_value2
    if (sendMessage(socket_cliente_descriptor, (char *) &N_value2_, sizeof(N_value2_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar el N_value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar operacion
    if (sendMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar value3.x
    if (sendMessage(socket_cliente_descriptor, (char *) &value3_x_, sizeof(value3_x_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar value3.x\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar value3.y
    if (sendMessage(socket_cliente_descriptor, (char *) &value3_y_, sizeof(value3_y_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar value3.y\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar value1
    if (sendMessage(socket_cliente_descriptor, peticion->value1, sizeof(peticion->value1)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar value1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar value2
    if (sendMessage(socket_cliente_descriptor, (char *) &v_value2_, sizeof(v_value2_)) == -1) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: enviar_peticion\nError: Error al enviar value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    return 0;
}

// Recibir respuesta get value
int recibir_get_value(int socket_cliente_descriptor, Peticion * resp){
    int32_t key_, N_value2_, operacion_, value3_x_, value3_y_;
    uint64_t v_value2_[32];

    // Recibir key
    if(recvMessage(socket_cliente_descriptor, (char *) &key_, sizeof(key_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir la key\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir N_value2
    if(recvMessage(socket_cliente_descriptor, (char *) &N_value2_, sizeof(N_value2_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir el N_value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value3.x
    if(recvMessage(socket_cliente_descriptor, (char *) &value3_x_, sizeof(value3_x_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir value3.x\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value3.y
    if(recvMessage(socket_cliente_descriptor, (char *) &value3_y_, sizeof(value3_y_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir value3.y\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Recibir value1
    if(recvMessage(socket_cliente_descriptor, resp->value1, sizeof(resp->value1)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir value1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value2
    if(recvMessage(socket_cliente_descriptor, (char*) &v_value2_, sizeof(v_value2_)) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: recibir_get_value\nError: Error al recibir value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Asignacion de valores a la estructura
    resp->key = ntohl(key_);
    resp->N_value2 = ntohl(N_value2_);
    resp->operacion = ntohl(operacion_);
    resp->value3.x = ntohl(value3_x_);
    resp->value3.y = ntohl(value3_y_);

    for (int i = 0; i < resp->N_value2; i++) {
        resp->value2[i] = deserializar_double(v_value2_[i]);
    }

    return 0;
}

int comunicacion_servidor(Peticion * peticion) {

    // Variables de entorno
    ip_servidor = getenv("IP_TUPLAS");
    puerto_servidor_char = getenv("PORT_TUPLAS");
    int puerto_servidor;
    
    // Comprobacion de las variables de entorno
    if (ip_servidor == NULL) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al obtener la variable de entorno IP_TUPLAS \n");
        return -1;
    }
    
    if (puerto_servidor_char == NULL) {
        perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al obtener la variable de entorno PORT_TUPLAS \n");
        return -1;
    }
    else{
        puerto_servidor = atoi(puerto_servidor_char);
        if(puerto_servidor == 0){
            if (strcmp(puerto_servidor_char, "0") != 0) {
                perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al obtener el puerto del servidor\n");
                return -1;
            }
        }
    }

    // Variables
    int respuesta;
    int socket_cliente_descriptor;
	struct sockaddr_in socket_servidor;	// socket del servidor
	struct hostent *datos_servidor;	// datos del servidor
	int32_t res;
	int error;
    
    // Socket del cliente
	socket_cliente_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_cliente_descriptor == -1) {
		perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al crear el socket del cliente\n");
        return -1;
	}

    // Socket del servidor
	bzero((char *)&socket_servidor, sizeof(socket_servidor));
   	datos_servidor = gethostbyname(ip_servidor);
	if (datos_servidor == NULL) {
		perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al obtener la informacion del servidor\n");
		close(socket_cliente_descriptor);
        return -1;
	}

    // Guardar los datos del servidor en la estructura para usarlos en el socket
    memcpy (&(socket_servidor.sin_addr), datos_servidor->h_addr_list[0], datos_servidor->h_length);
    socket_servidor.sin_family  = AF_INET;
    socket_servidor.sin_port    = htons(puerto_servidor);
    
    // Conectar el socket del cliente con el servidor
    error = connect(socket_cliente_descriptor, (struct sockaddr *) &socket_servidor,  sizeof(socket_servidor));
	if (error == -1) {
		perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al conectar el socket del cliente con el servidor\n");
		close(socket_cliente_descriptor);
        return -1;
	}

    // Enviar datos al servidor
    printf("\nEnviando peticion.\n");
    if (enviar_peticion(socket_cliente_descriptor, peticion) == -1){
        perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al enviar la peticion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir respuesta
    if (peticion->operacion == 2){      // Funcion get_value
        Peticion resp;
        printf("Recibiendo respuesta del servidor.\n");
        if (recibir_get_value(socket_cliente_descriptor, &resp) == -1){
            perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error al recibir la respuesta\n");
            close(socket_cliente_descriptor);
            return -1;
        }
        if (resp.N_value2 == -1){
            perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error en la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
        strcpy(peticion->value1, resp.value1);
        peticion->N_value2 = resp.N_value2;
        memcpy(&peticion->value2, &resp.value2, (peticion->N_value2)*sizeof(double));
        peticion->value3 = resp.value3;
    }
    else{   // Cualquier otra funcion
        printf("Recibiendo respuesta del servidor.\n");
        if(recvMessage(socket_cliente_descriptor, (char *) &res, sizeof(int32_t)) == -1) {
            perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
        respuesta = ntohl(res);
        if (respuesta == -1){
            perror("ERROR:\nFile: proxy-sock.c\nFunction: tratar_peticion\nError: Error en la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
    }

    // Cerrar socket
    close(socket_cliente_descriptor);
    return 0;
}

// Funcion para registrar un nuevo usuario
int register_user(char* username) {

}

// Funcion para dar de baja a un usuario
int unregister(char* username) {

}

// Funcion para conectar al cliente al sistema
int connect(char* username) {
    
}

// Funcion para desconectar al cliente del sistema
int disconnect(char* username) {
    
}

// Funcion para publicar un fichero
int publish(char * username_publisher, char* file_name, char* descripcion) {
    
}

// Funcion para borrar un fichero
int delete(char * username_solicitante, char * file_name) {
    
}

// Funcion para obtener los usuarios
int listusers(void) {
    
}

// Funcion para obtener los ficheros
int listcontent(char * username) {
    
}

// Funcion para obtener un fichero
int get_file(char * username, char * remote_filename, char * local_filename) {
    
}