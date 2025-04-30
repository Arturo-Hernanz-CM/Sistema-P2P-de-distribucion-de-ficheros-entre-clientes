#include "claves.h"
#include "estructuras.h"
#include "lines.h"
#include <stdint.h>
#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Variables globales
char *ip_servidor;
char *puerto_servidor_char;
Lista * base_datos = NULL;

pthread_mutex_t mutex_mensaje;
pthread_cond_t condicion_mensaje;
pthread_mutex_t mutex_funcion;
int ocupado = 0;

int puerto_cliente;
char ip_cliente[INET_ADDRSTRLEN];

// Funciones

// Recibir la peticion del cliente
int recibir_peticion(int socket_cliente_descriptor, Peticion * peticion_local){
    int32_t operacion_;

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir username
    if(recvMessage(socket_cliente_descriptor, peticion_local->username, sizeof(peticion_local->username)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir username\n");
        close(socket_cliente_descriptor);
        return -1;
    }
    
    // Recibir username2
    if(recvMessage(socket_cliente_descriptor, peticion_local->username2, sizeof(peticion_local->username2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir username2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename1
    if(recvMessage(socket_cliente_descriptor, peticion_local->filename1, sizeof(peticion_local->filename1)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir filename1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir descripcion    
    if(recvMessage(socket_cliente_descriptor, peticion_local->description, sizeof(peticion_local->description)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir la descripcion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename2
    if(recvMessage(socket_cliente_descriptor, peticion_local->filename2, sizeof(peticion_local->filename2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir filename2\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Asignacion de valores a la estructura
    peticion_local->operacion = ntohl(operacion_);

    return 0;
}

// Funcion que recibe las peticiones
void recibir_socket(int *socket_cliente_descriptor){
    // Variables
    int socket_cliente_descriptor_local;
    Conexion conexion;
    Peticion peticion_local = {0};
    int respuesta;
    int32_t res;
    int32_t resp;
    char * user_ip_port = NULL;
    char * filename = NULL;
    Nodo * nodo = NULL;

    // Copiar el descriptor del socket del cliente
    pthread_mutex_lock(&mutex_mensaje);
    socket_cliente_descriptor_local = (*(int *) socket_cliente_descriptor);
    ocupado = 1;
    pthread_cond_signal(&condicion_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    // Recibe los datos del cliente
    if (recibir_peticion(socket_cliente_descriptor_local, &peticion_local)){
        perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al recibir la petición\n");
        close(socket_cliente_descriptor_local);
        respuesta = -1;
    }

    // Ejecutar la petición
    if (peticion_local.operacion == 0) {    // Función register
        respuesta = register_user(peticion_local.username);
    }
    else if (peticion_local.operacion == 1) {   // Función unregister
        respuesta = unregister(peticion_local.username);
    }
    else if (peticion_local.operacion == 2) {   // Función connect
        respuesta = connect_user(peticion_local.username, &puerto_cliente, ip_cliente);
    }
    else if (peticion_local.operacion == 3) {   // Función disconnect
        respuesta = disconnect(peticion_local.username);
    }
    else if (peticion_local.operacion == 4) {   // Función publish
        respuesta = publish(peticion_local.username, peticion_local.filename1, peticion_local.description);
    }
    else if (peticion_local.operacion == 5) {   // Función delete
        respuesta = delete(peticion_local.username, peticion_local.filename1);
    }
    else if (peticion_local.operacion == 6){
        conexion = is_connected(peticion_local.username);
        respuesta = conexion.error;
    }
    else if(peticion_local.operacion == 7){
        conexion = is_connected(peticion_local.username);
        respuesta = conexion.error;
    }
    else {
        respuesta = -1;
    }

    // Enviar la respuesta
    printf("Enviando respuesta a cliente: %d\n", socket_cliente_descriptor_local);
    
    res = htonl(respuesta);
    if(peticion_local.operacion == 6) {
        if(respuesta == -1){
            resp = 0;
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp, sizeof(resp)) == -1) {
                perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }  
        }
        else{
            resp = base_datos->size;
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp, sizeof(resp)) == -1) {
                perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                close(socket_cliente_descriptor_local);
            }  
            nodo = base_datos->inicio;
            for (int i = 0; i < resp; i++){
                int total_size = strlen(nodo->username) + 1 + strlen(nodo->datos_conexion->ip) + 1 + sizeof(nodo->datos_conexion->port) + 1;
                user_ip_port = (char *) malloc(sizeof(nodo->username) + sizeof(nodo->datos_conexion->ip) + sizeof(nodo->datos_conexion->port));
                if(user_ip_port == NULL){
                    perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al reservar memoria");
                    close(socket_cliente_descriptor_local);
                }

                snprintf(user_ip_port, total_size, "%s %s %d", nodo->username, nodo->datos_conexion->ip, *nodo->datos_conexion->port);

                if(sendMessage(socket_cliente_descriptor_local, (char*) &user_ip_port, sizeof(user_ip_port)) == -1) {
                    perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                    close(socket_cliente_descriptor_local);
                } 
                free(user_ip_port);
            }
        }

    }
    else if(peticion_local.operacion == 7){
        if(respuesta == -1){
            resp = 0;
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp, sizeof(resp)) == -1) {
                perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }  
        }
    }
    else{
        if(sendMessage(socket_cliente_descriptor_local, (char *) &res, sizeof(res)) == -1) {
            perror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
            close(socket_cliente_descriptor_local);
        }  
    }     
}

int main(int argc, char **argv) {
    // Comprobacion de argumentos
    if(argc != 2) {
        perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Número de argumentos incorrecto\n");
        return -1;
    }

    // Inicializar la base de datos
    base_datos = (Lista *) malloc(sizeof(Lista));
    if (base_datos == NULL) {
        perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al reservar memoria para la lista de claves");
        return -1;
    }
    base_datos->inicio = NULL;
    base_datos->final = NULL;
    base_datos->size = 0;

    // Inicializar estructuras
    struct sockaddr_in socket_servidor,  socket_cliente;	// estructuras para info de los sockets

    // Variables para los sockets
    socklen_t size;
    int socket_servidor_descriptor, socket_cliente_descriptor;
    int val;
    int err;
    int puerto_servidor;

    // Inicializar hilos
    pthread_t thread;
    pthread_attr_t thread_att;
    pthread_attr_init(&thread_att);

    // Inicializar mutex
    pthread_mutex_init(&mutex_mensaje, NULL);
    pthread_cond_init(&condicion_mensaje, NULL);
    pthread_mutex_init(&mutex_funcion, NULL);
    
    // Crear socket servidor
	if ((socket_servidor_descriptor =  socket(AF_INET, SOCK_STREAM, 0))<0){
        perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al crear el socket del servidor");
		return 0;
	}
	val = 1;
	setsockopt(socket_servidor_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	// Guardar la info del socket del servidor en la estructura
    puerto_servidor = atoi(argv[1]);
    if(puerto_servidor == 0) {
        if(strcmp(argv[1], "0") != 0) {
            perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al obtener el puerto del servidor\n");
            return -1;
        }
    }

	bzero((char *)&socket_servidor, sizeof(socket_servidor));
	socket_servidor.sin_family      = AF_INET;
    socket_servidor.sin_addr.s_addr = INADDR_ANY;
	socket_servidor.sin_port        = htons(puerto_servidor);

	// Asignar los datos del servidor al socket del servidor
	err = bind(socket_servidor_descriptor, (const struct sockaddr *)&socket_servidor, sizeof(socket_servidor));
	if (err == -1) {
        perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al asignar los datos del servidor al socket del servidor");
		return -1;
	}

    // Poner el socket del servidor en modo de escucha
	err = listen(socket_servidor_descriptor, SOMAXCONN);
	if (err == -1) {
        perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al poner el socket del servidor en modo de escucha");
		return -1;
	}

    // Peticiones
	pthread_attr_setdetachstate(&thread_att, PTHREAD_CREATE_DETACHED);      // threads independientes
    while(1) {
        // Aceptar la conexion
        socket_cliente_descriptor = accept(socket_servidor_descriptor, (struct sockaddr *)&socket_cliente, &size);
        if (socket_cliente_descriptor < 0) {
            perror("ERROR:\nFile: servidor.c\nFunction: main\nError: Error al aceptar la conexion");
            return -1;
        }

        // Info del cliente
        inet_ntop(AF_INET, &(socket_cliente.sin_addr), ip_cliente, INET_ADDRSTRLEN);
        puerto_cliente = ntohs(socket_cliente.sin_port);
        printf("Conexión de cliente %s:%d\n", ip_cliente, puerto_cliente);

        // Recibir peticion
        printf("\nRecibiendo peticion del cliente.\n");
        if(pthread_create(&thread, &thread_att, (void*) recibir_socket, &socket_cliente_descriptor) == 0) {
            pthread_mutex_lock(&mutex_mensaje);
            while(ocupado == 0) {
                pthread_cond_wait(&condicion_mensaje, &mutex_mensaje);
            }
            ocupado = 0;
            pthread_mutex_unlock(&mutex_mensaje);
        }
    }

    // Cerrar el socket
    close(socket_servidor_descriptor);
    return 0;
}