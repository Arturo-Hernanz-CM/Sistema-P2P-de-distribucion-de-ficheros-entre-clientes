#include "proxy.h"
#include "lines.h"
#include <stdint.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>


// Variable global
Peticion peticion = {0};
int hilo = 0;
char *ip_servidor = NULL;
char *puerto_servidor_char = NULL;

// Inicializar hilo0
pthread_t thread;
pthread_attr_t thread_att;


// Funciones

// Enviar peticion
int enviar_peticion(int socket_cliente_descriptor, Peticion * peticion){
   int32_t operacion_;

   operacion_ = htonl(peticion->operacion);

   // Enviar operacion
    if (sendMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar username
    if (sendMessage(socket_cliente_descriptor, peticion->username, sizeof(peticion->username)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar username\n");
        close(socket_cliente_descriptor);
        return -1;
    }
    
    // Enviar username2
    if (sendMessage(socket_cliente_descriptor, peticion->username2, sizeof(peticion->username2)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar username2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar filename1
    if (sendMessage(socket_cliente_descriptor, peticion->filename1, sizeof(peticion->filename1)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar filename1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar descripcion
    if (sendMessage(socket_cliente_descriptor, peticion->description, sizeof(peticion->description)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar la descripcion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar filename2
    if (sendMessage(socket_cliente_descriptor, peticion->filename2, sizeof(peticion->filename2)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar filename2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    return 0;
}

// Recibir respuesta get value
int recibir_get_value(int socket_cliente_descriptor, Peticion * resp){
    int32_t operacion_;

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir username
    if(recvMessage(socket_cliente_descriptor, resp->username, sizeof(resp->username)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir username\n");
        close(socket_cliente_descriptor);
        return -1;
    }
    
    // Recibir username2
    if(recvMessage(socket_cliente_descriptor, resp->username2, sizeof(resp->username2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir username2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename1
    if(recvMessage(socket_cliente_descriptor, resp->filename1, sizeof(resp->filename1)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir filename1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir descripcion    
    if(recvMessage(socket_cliente_descriptor, resp->description, sizeof(resp->description)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir la descripcion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename2
    if(recvMessage(socket_cliente_descriptor, resp->filename2, sizeof(resp->filename2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_get_value\nError: Error al recibir filename2\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Asignacion de valores a la estructura
    resp->operacion = ntohl(operacion_);

    return 0;
}

int comunicacion_servidor(Peticion * peticion) {
    // Variables
    int puerto_servidor;
    
    // Comprobacion de las variables
    puerto_servidor = atoi(puerto_servidor_char);
    if(puerto_servidor == 0){
        if (strcmp(puerto_servidor_char, "0") != 0) {
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al obtener el puerto del servidor\n");
            return -1;
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
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al crear el socket del cliente\n");
        return -1;
	}

    // Socket del servidor
	bzero((char *)&socket_servidor, sizeof(socket_servidor));
   	datos_servidor = gethostbyname(ip_servidor);
	if (datos_servidor == NULL) {
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al obtener la informacion del servidor\n");
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
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al conectar el socket del cliente con el servidor\n");
		close(socket_cliente_descriptor);
        return -1;
	}

    // Enviar datos al servidor
    printf("\nEnviando peticion.\n");
    if (enviar_peticion(socket_cliente_descriptor, peticion) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al enviar la peticion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir respuesta
    /*if (peticion->operacion == 2){      // Funcion get_value
        Peticion resp;
        printf("Recibiendo respuesta del servidor.\n");
        if (recibir_get_value(socket_cliente_descriptor, &resp) == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al recibir la respuesta\n");
            close(socket_cliente_descriptor);
            return -1;
        }
        if (resp.N_value2 == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error en la respuesta");
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
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
        respuesta = ntohl(res);
        if (respuesta == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error en la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
    }*/

    if(peticion->operacion == 6 || peticion->operacion == 7){
        char respuesta;
        int fin_emit;
        int32_t fin_emit_;
        if(recvMessage(socket_cliente_descriptor, (char *) &fin_emit_, sizeof(fin_emit_)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
        fin_emit = ntohl(fin_emit_);
        for(int i = 0; i < fin_emit; i++){
            if(recvMessage(socket_cliente_descriptor, (char *) &respuesta, sizeof(respuesta)) == -1) {
                perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
                close(socket_cliente_descriptor);
                return -1;
            }
            printf("%c\n", respuesta);
        }
        
    }
    else{
        printf("Recibiendo respuesta del servidor.\n");
        if(recvMessage(socket_cliente_descriptor, (char *) &res, sizeof(int32_t)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
        respuesta = ntohl(res);
        if (respuesta == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: Error en la respuesta");
            close(socket_cliente_descriptor);
            return -1;
        }
    }

    // Cerrar socket
    close(socket_cliente_descriptor);
    return 0;
}

// Funcion para registrar un nuevo usuario
int register_user(char* username, char* ip_s, char* port_s) {
    // Crear el hilo para escuchar peticiones de otros usuarios si no está creado
    if(hilo == 0){
        /*pthread_attr_init(&thread_att);
        if(pthread_create(&thread, &thread_att, (void*) recibir_socket, &socket_cliente_descriptor) == 0) {
            pthread_mutex_lock(&mutex_mensaje);
            while(ocupado == 0) {
                pthread_cond_wait(&condicion_mensaje, &mutex_mensaje);
            }
            ocupado = 0;
            pthread_mutex_unlock(&mutex_mensaje);
        }*/
        hilo = 1;
    }

    ip_servidor = malloc(sizeof(char) * (strlen(ip_s) + 1));
    if(ip_servidor == NULL){
        perror("ERROR:\nFile: proxy.c\nFunction: register_user\nError: Error al reservar memoria para guardar la ip del servidor");
        return -1;
    }
    strcpy(ip_servidor, ip_s);

    puerto_servidor_char = malloc(sizeof(char) * (strlen(port_s) + 1));
    if(puerto_servidor_char == NULL){
        perror("ERROR:\nFile: proxy.c\nFunction: register_user\nError: Error al reservar memoria para guardar el puerto del servidor");
        free(ip_servidor);
        return -1;
    }
    strcpy(puerto_servidor_char, port_s);

    strcpy(peticion.username, username);
    peticion.operacion = 0;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("USERNAME IN USE\n");
        free(ip_servidor);
        free(puerto_servidor_char);
        return -1;
    }
    else if(resultado == 2){
        perror("REGISTER FAIL\n");
        free(ip_servidor);
        free(puerto_servidor_char);
        return -1;
    }
    printf("REGISTER OK\n");
    return 0;
}

// Funcion para dar de baja a un usuario
int unregister(char* username) {
    strcpy(peticion.username, username);
    peticion.operacion = 1;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("UNREGISTER FAIL\n");
        return -1;
    }

    printf("UNREGISTER OK\n");
    free(ip_servidor);
    free(puerto_servidor_char);
    return 0;
}

// Funcion para conectar al cliente al sistema
int connect(char* username) {
    strcpy(peticion.username, username);
    peticion.operacion = 2;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("CONNECT FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("USER ALREADY CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("CONNECT FAIL\n");
        return -1;
    }

    printf("CONNECT OK\n");
    return 0;
}

// Funcion para desconectar al cliente del sistema
int disconnect(char* username) {
    strcpy(peticion.username, username);
    peticion.operacion = 3;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("DISCONNECT FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("DISCONNECT FAIL, USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("DISCONNECT FAIL\n");
        return -1;
    }

    printf("DISCONNECT OK\n");
    free(ip_servidor);
    free(puerto_servidor_char);
    return 0;
}

// Funcion para publicar un fichero
int publish(char * username_publisher, char* file_name, char* descripcion) {
    strcpy(peticion.username, username_publisher);
    strcpy(peticion.filename1, file_name);
    strcpy(peticion.description, descripcion);
    peticion.operacion = 4;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("PUBLISH FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("PUBLISH FAIL, USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("PUBLISH FAIL, CONTENT ALREADY PUBLISHED\n");
        return -1;
    }
    else if(resultado == 4){
        perror("PUBLISH FAIL\n");
        return -1;
    }

    printf("PUBLISH OK\n");
    return 0;
}

// Funcion para borrar un fichero
int delete(char * username_solicitante, char * file_name) {
    strcpy(peticion.username, username_solicitante);
    strcpy(peticion.filename1, file_name);
    peticion.operacion = 5;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("DELETE FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("DELETE FAIL, USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("DELETE FAIL, CONTENT NOT PUBLISHED\n");
        return -1;
    }
    else if(resultado == 4){
        perror("DELETE FAIL\n");
        return -1;
    }

    printf("DELETE OK\n");
    return 0;
}

// Funcion para obtener los usuarios
int listusers(char * username) {
    strcpy(peticion.username, username);
    peticion.operacion = 6;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("LISTUSERS FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("LISTUSERS FAIL, USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("LISTUSERS FAIL\n");
        return -1;
    }

    printf("LISTUSERS OK\n");
    return 0;
}

// Funcion para obtener los ficheros
int listcontent(char * username, char * username_solicitante) {
    strcpy(peticion.username, username);
    strcpy(peticion.username2, username_solicitante);
    peticion.operacion = 7;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("LISTCONTENT FAIL, USER DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("LISTCONTENT FAIL, USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 3){
        perror("LISTCONTENT FAIL, REMOTE USER NOT CONNECTED\n");
        return -1;
    }
    else if(resultado == 4){
        perror("LISTCONTENT FAIL\n");
        return -1;
    }

    printf("LISTCONTENT OK\n");
    return 0;
}

// Funcion para obtener un fichero
int get_file(char * username, char * remote_filename, char * local_filename, char * ip_dest, int port_dest) {
    strcpy(peticion.username, username);
    strcpy(peticion.filename1, remote_filename);
    strcpy(peticion.filename2, local_filename);
    peticion.operacion = 8;

    int resultado = comunicacion_servidor(&peticion);
    if(resultado == 1){
        perror("GETFILE FAIL, FILE DOES NOT EXIST\n");
        return -1;
    }
    else if(resultado == 2){
        perror("GETFILE FAIL\n");
        return -1;
    }

    printf("GETFILE OK\n");
    return 0;
}