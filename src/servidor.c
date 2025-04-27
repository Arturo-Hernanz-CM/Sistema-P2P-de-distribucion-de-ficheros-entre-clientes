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

// Funciones

// Serializar double
uint64_t serializar_double(double value){
    uint64_t v_value2;
    memcpy(&v_value2, &value, sizeof(value));
    return htobe64(v_value2);
}

// Deserializar double
double deserializar_double(uint64_t v_value2){
    uint64_t value2;
    double result;

    value2 = be64toh(v_value2);
    memcpy(&result, &value2, sizeof(value2));
    return result;
}

// Recibir la peticion del cliente
int recibir_peticion(int socket_cliente_descriptor, Peticion * peticion_local){
    int32_t key_, N_value2_, operacion_, value3_x_, value3_y_;
    uint64_t v_value2_[32];

    // Recibir key
    if(recvMessage(socket_cliente_descriptor, (char *) &key_, sizeof(key_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir la key\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir N_value2
    if(recvMessage(socket_cliente_descriptor, (char *) &N_value2_, sizeof(N_value2_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir el N_value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value3.x
    if(recvMessage(socket_cliente_descriptor, (char *) &value3_x_, sizeof(value3_x_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir value3.x\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value3.y
    if(recvMessage(socket_cliente_descriptor, (char *) &value3_y_, sizeof(value3_y_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir value3.y\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Recibir value1
    if(recvMessage(socket_cliente_descriptor, peticion_local->value1, sizeof(peticion_local->value1)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir value1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir value2
    if(recvMessage(socket_cliente_descriptor, (char *) &v_value2_, sizeof(v_value2_)) == -1){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_peticion\nError: Error al recibir value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Asignacion de valores a la estructura
    peticion_local->key = ntohl(key_);
    peticion_local->N_value2 = ntohl(N_value2_);
    peticion_local->operacion = ntohl(operacion_);
    peticion_local->value3.x = ntohl(value3_x_);
    peticion_local->value3.y = ntohl(value3_y_);

    for (int i = 0; i < peticion_local->N_value2; i++) {
        peticion_local->value2[i] = deserializar_double(v_value2_[i]);
    }

    return 0;
}

// Enviar la respuesta al cliente si la operacion es get_value
int enviar_respuesta_get_value(int socket_cliente_descriptor, Peticion * resp){
    int32_t key_, N_value2_, operacion_, value3_x_, value3_y_;
    uint64_t v_value2_[32];

    key_ = htonl(resp->key);
    N_value2_ = htonl(resp->N_value2);
    operacion_ = htonl(resp->operacion);
    value3_x_ = htonl(resp->value3.x);
    value3_y_ = htonl(resp->value3.y);

    for (int i = 0; i < resp->N_value2; i++) {
        v_value2_[i] = serializar_double(resp->value2[i]);
    }
 
    // Enviar key
    if (sendMessage(socket_cliente_descriptor, (char *) &key_, sizeof(key_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar la clave\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    // Enviar N_value2
    if (sendMessage(socket_cliente_descriptor, (char *) &N_value2_, sizeof(N_value2_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar el N_value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    // Enviar operacion
    if (sendMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
     // Enviar value3.x
    if (sendMessage(socket_cliente_descriptor, (char *) &value3_x_, sizeof(value3_x_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar value3.x\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    // Enviar value3.y
    if (sendMessage(socket_cliente_descriptor, (char *) &value3_y_, sizeof(value3_y_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar value3.y\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    // Enviar value1
    if (sendMessage(socket_cliente_descriptor, resp->value1, sizeof(resp->value1)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar value1\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    // Enviar value2
    if (sendMessage(socket_cliente_descriptor, (char *) &v_value2_, sizeof(v_value2_)) == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: enviar_rep_get_value\nError: Error al enviar value2\n");
        close(socket_cliente_descriptor);
        return -1;
    }
 
    return 0;  
}

// Funcion que recibe las peticiones
void recibir_socket(int *socket_cliente_descriptor){
    // Variables
    int socket_cliente_descriptor_local;
    Peticion peticion_local = {0};
    Peticion resp = {0};
    int respuesta;
    int32_t res;

    // Copiar el descriptor del socket del cliente
    pthread_mutex_lock(&mutex_mensaje);
    socket_cliente_descriptor_local = (*(int *) socket_cliente_descriptor);
    ocupado = 1;
    pthread_cond_signal(&condicion_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    // Recibe los datos del cliente
    if (recibir_peticion(socket_cliente_descriptor_local, &peticion_local)){
        perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_socket\nError: Error al recibir la petición\n");
        close(socket_cliente_descriptor_local);
        respuesta = -1;
    }

    // Ejecutar la petición
    if (peticion_local.operacion == 0) {    // Función destroy
        respuesta = destroy();
    }
    else if (peticion_local.operacion == 1) {   // Función set_value
        respuesta = set_value(peticion_local.key, peticion_local.value1, peticion_local.N_value2, peticion_local.value2, peticion_local.value3);
    }
    else if (peticion_local.operacion == 2) {   // Función get_value
        resp.key = peticion_local.key;
        respuesta = get_value(resp.key, resp.value1, &resp.N_value2, resp.value2, &resp.value3);
    }
    else if (peticion_local.operacion == 3) {   // Función modify_value
        respuesta = modify_value(peticion_local.key, peticion_local.value1, peticion_local.N_value2, peticion_local.value2, peticion_local.value3);
    }
    else if (peticion_local.operacion == 4) {   // Función delete_key
        respuesta = delete_key(peticion_local.key);
    }
    else if (peticion_local.operacion == 5) {   // Función exist
        respuesta = exist(peticion_local.key);
    }
    else {
        respuesta = -1;
    }

    // Enviar la respuesta
    printf("Enviando respuesta a cliente: %d\n", socket_cliente_descriptor_local);
    
    res = htonl(respuesta);
    if(peticion_local.operacion == 2) {
        if (respuesta == -1) {
            resp = peticion_local;
            resp.N_value2 = -1;
        }
        if(enviar_respuesta_get_value(socket_cliente_descriptor_local, &resp) == -1) {
            perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
            close(socket_cliente_descriptor_local);
        }  
    }
    else{
        if(sendMessage(socket_cliente_descriptor_local, (char *) &res, sizeof(res)) == -1) {
            perror("ERROR:\nFile: servidor-sock.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
            close(socket_cliente_descriptor_local);
        }  
    }     
}

int main(int argc, char **argv) {
    // Comprobacion de argumentos
    if(argc != 2) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Número de argumentos incorrecto\n");
        return -1;
    }

    // Inicializar la base de datos
    base_datos = (Lista *) malloc(sizeof(Lista));
    if (base_datos == NULL) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al reservar memoria para la lista de claves");
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
        perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al crear el socket del servidor");
		return 0;
	}
	val = 1;
	setsockopt(socket_servidor_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	// Guardar la info del socket del servidor en la estructura
    puerto_servidor = atoi(argv[1]);
    if(puerto_servidor == 0) {
        if(strcmp(argv[1], "0") != 0) {
            perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al obtener el puerto del servidor\n");
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
        perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al asignar los datos del servidor al socket del servidor");
		return -1;
	}

    // Poner el socket del servidor en modo de escucha
	err = listen(socket_servidor_descriptor, SOMAXCONN);
	if (err == -1) {
        perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al poner el socket del servidor en modo de escucha");
		return -1;
	}

    // Peticiones
	pthread_attr_setdetachstate(&thread_att, PTHREAD_CREATE_DETACHED);      // threads independientes
    while(1) {
        // Aceptar la conexion
        socket_cliente_descriptor = accept(socket_servidor_descriptor, (struct sockaddr *)&socket_cliente, &size);
        if (socket_cliente_descriptor < 0) {
            perror("ERROR:\nFile: servidor-sock.c\nFunction: main\nError: Error al aceptar la conexion");
            return -1;
        }

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