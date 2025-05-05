#include "claves.h"
#include "estructuras.h"
#include "lines.h"
#include "tuplas.h"
#include <stdint.h>
#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Variables globales
char ip_servidor[INET_ADDRSTRLEN];
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
    int32_t operacion_, port_;

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir puerto
    if(recvMessage(socket_cliente_descriptor, (char *) &port_, sizeof(port_)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir el puerto\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir username
    if(recvMessage(socket_cliente_descriptor, peticion_local->username, sizeof(peticion_local->username)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir username\n");
        close(socket_cliente_descriptor);
        return -1;
    }
    
    // Recibir username2
    if(recvMessage(socket_cliente_descriptor, peticion_local->username2, sizeof(peticion_local->username2)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir username2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename1
    if(recvMessage(socket_cliente_descriptor, peticion_local->filename1, sizeof(peticion_local->filename1)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir filename1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir descripcion    
    if(recvMessage(socket_cliente_descriptor, peticion_local->description, sizeof(peticion_local->description)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir la descripcion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename2
    if(recvMessage(socket_cliente_descriptor, peticion_local->filename2, sizeof(peticion_local->filename2)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir filename2\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Recibir fechahora
    if(recvMessage(socket_cliente_descriptor, peticion_local->fecha_hora, sizeof(peticion_local->fecha_hora)) == -1){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_peticion\nError: Error al recibir la fechahora\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Asignacion de valores a la estructura
    peticion_local->operacion = ntohl(operacion_);
    peticion_local->port_escucha = ntohl(port_);

    return 0;
}

// Funcion que recibe las peticiones
void recibir_socket(int *socket_cliente_descriptor){
    // Variables
    int socket_cliente_descriptor_local;
    Conexion conexion, conexion2;
    Peticion peticion_local = {0};
    int respuesta = 0;
    int32_t res;
    int32_t resp;
    char * user_ip_port = NULL;
    Nodo * nodo = NULL;
    Publicacion * publicacion = NULL;
    int error = 0;
    int rcp;
    arg_log log = {0};
    arg_log_filename log_filename = {0};

    // Copiar el descriptor del socket del cliente
    pthread_mutex_lock(&mutex_mensaje);
    socket_cliente_descriptor_local = (*(int *) socket_cliente_descriptor);
    ocupado = 1;
    pthread_cond_signal(&condicion_mensaje);
    pthread_mutex_unlock(&mutex_mensaje);

    // Recibe los datos del cliente
    if (recibir_peticion(socket_cliente_descriptor_local, &peticion_local)){
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al recibir la petición\n");
        close(socket_cliente_descriptor_local);
        respuesta = -5;
    }

    // Cliente para el servidor rcp
    CLIENT *cliente = clnt_create(ip_servidor, TUPLAS, TUPLASVER, "tcp");
    if(cliente == NULL){
        clnt_pcreateerror("ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al crear el cliente");
        respuesta = -5;
    }

    // Ejecutar la petición
    if (peticion_local.operacion == 0) {    // Función register
        printf("\ns> OPERATION REGISTER FROM %s\n", peticion_local.username);

        // LLamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "REGISTER";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al registrar el usuario en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if (respuesta == 0){
            respuesta = register_user(peticion_local.username);
        }
        error = -2;
    }
    else if (peticion_local.operacion == 1) {   // Función unregister
        printf("\ns> OPERATION UNREGISTER FROM %s\n", peticion_local.username);

        // LLamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "UNREGISTER";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al desregistrar el usuario en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            respuesta = unregister(peticion_local.username);
        }
        error = -2;
    }
    else if (peticion_local.operacion == 2) {   // Función connect
        printf("\ns> OPERATION CONNECT FROM %s\n", peticion_local.username);

        // LLamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "CONNECT";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al conectar el usuario en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            respuesta = connect_user(peticion_local.username, peticion_local.port_escucha, ip_cliente);
        }
        error = -3;
    }
    else if (peticion_local.operacion == 3) {   // Función disconnect
        printf("\ns> OPERATION DISCONNECT FROM %s\n", peticion_local.username);

        // LLamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "DISCONNECT";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al desconectar el usuario en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            respuesta = disconnect(peticion_local.username);
        }
        error = -3;
    }
    else if (peticion_local.operacion == 4) {   // Función publish
        printf("\ns> OPERATION PUBLISH FROM %s\n", peticion_local.username);

        // LLamada RCP
        log_filename.nombre = peticion_local.username;
        log_filename.operacion = "PUBLISH";
        log_filename.filename = peticion_local.filename1;
        log_filename.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_filename_1(log_filename, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al publicar el archivo en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            respuesta = publish(peticion_local.username, peticion_local.filename1, peticion_local.description);
        }
        error = -4;
    }
    else if (peticion_local.operacion == 5) {   // Función delete
        printf("\ns> OPERATION DELETE FROM %s\n", peticion_local.username);

        // LLamada RCP
        log_filename.nombre = peticion_local.username;
        log_filename.operacion = "DELETE";
        log_filename.filename = peticion_local.filename1;
        log_filename.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_filename_1(log_filename, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al eliminar el archivo en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            respuesta = delete(peticion_local.username, peticion_local.filename1);
        }
        error = -4;
    }
    else if (peticion_local.operacion == 6){    // Función listusers
        printf("\ns> OPERATION LIST_USERS FROM %s\n", peticion_local.username);

        // Llamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "LIST_USERS";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al listar los usuarios en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            conexion = is_connected(peticion_local.username);
            respuesta = conexion.error;
        }
        error = -3;
    }
    else if(peticion_local.operacion == 7){     // Función listcontent
        printf("\ns> OPERATION LIST_CONTENT FROM %s\n", peticion_local.username);

        // Llamada RCP
        log.nombre = peticion_local.username;
        log.operacion = "LIST_CONTENT";
        log.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_1(log, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al listar el contenido en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            conexion = is_connected(peticion_local.username);
            conexion2 = is_connected(peticion_local.username2);
            if(conexion2.error == -2){
            respuesta = -3; 
            }
            else{
                respuesta = conexion.error;
            }
        }
        
        error = -4;
    }
    else if(peticion_local.operacion == 8){     // Función getfile
        printf("\ns> OPERATION GET_VALUE FROM %s\n", peticion_local.username);

        // Llamada RCP
        log_filename.nombre = peticion_local.username;
        log_filename.operacion = "GET_VALUE";
        log_filename.filename = peticion_local.filename1;
        log_filename.fecha_hora = peticion_local.fecha_hora;

        rcp = *log_filename_1(log_filename, cliente);
        if(rcp == -1){
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al obtener el archivo en el servidor rcp");
            respuesta = -5;
        }

        // Llamada al servidor
        if(respuesta == 0){
            conexion = is_connected(peticion_local.username);
            conexion2 = is_connected(peticion_local.username2);
            if(conexion.error < 0){
            respuesta = -1; 
            }
            else if(conexion2.error < 0){
            respuesta = -1; 
            }
            else{
                respuesta = 0;
            }

            if(respuesta > 0){
                Published published = is_published(peticion_local.filename1, conexion.exist.nodo_usuario->publicaciones);
                if(published.error < 0){
                    respuesta = -1;
                }
            }
        }
        error = -4;
    }
    else {
        respuesta = -5;
    }

    // Enviar la respuesta
    printf("\ns> Enviando respuesta a cliente: %d\n", socket_cliente_descriptor_local);
    
    if(respuesta <= error){
        respuesta = error;
    }
    res = htonl(respuesta);
    if(peticion_local.operacion == 6) { // Función listusers
        if(respuesta < 0){
            resp = 0;
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp, sizeof(resp)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }  
        }
        else{
            resp = base_datos->size;
            int32_t resp2 = htonl(resp);
            int32_t tot_size;
            
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp2, sizeof(resp2)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                close(socket_cliente_descriptor_local);
            }  
            nodo = base_datos->inicio;
            for (int i = 0; i < resp; i++){
                int total_size = strlen(nodo->username) + 1 + strlen(nodo->datos_conexion->ip) + 1 + 6; // max 5 digits port + null
                user_ip_port = (char *) malloc(total_size);
                if(user_ip_port == NULL){
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al reservar memoria");
                    close(socket_cliente_descriptor_local);
                }

                tot_size = htonl(total_size);
                if(sendMessage(socket_cliente_descriptor_local, (char*) &tot_size, sizeof(tot_size)) == -1) {
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                    close(socket_cliente_descriptor_local);
                    free(user_ip_port);
                }

                snprintf(user_ip_port, total_size, "%s %s %d", nodo->username, nodo->datos_conexion->ip, nodo->datos_conexion->port);

                if(sendMessage(socket_cliente_descriptor_local, user_ip_port, strlen(user_ip_port)+1) == -1) {
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                    close(socket_cliente_descriptor_local);
                    free(user_ip_port);
                }
                free(user_ip_port);
                nodo = nodo->siguiente;
            }
        }

    }
    else if(peticion_local.operacion == 7){ // Función listcontent
        if(respuesta < 0){
            resp = 0;
            int32_t resp2 = htonl(resp);
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp2, sizeof(resp2)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }  
        }
        else{
            nodo = conexion.exist.nodo_usuario;
            if(nodo->publicaciones == NULL){
                resp = 0;
            }
            resp = nodo->publicaciones->size;
            int32_t resp3 = htonl(resp);
            int32_t tot_size;
            
            if(sendMessage(socket_cliente_descriptor_local, (char*) &resp3, sizeof(resp3)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                close(socket_cliente_descriptor_local);
            }  
            if(resp>0){
                publicacion = nodo->publicaciones->inicio;
            }
            for (int i = 0; i < resp; i++){
                int total_size = strlen(publicacion->filename) + 1; // max 5 digits port + null
                user_ip_port = (char *) malloc(total_size);
                if(user_ip_port == NULL){
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: Error al reservar memoria");
                    close(socket_cliente_descriptor_local);
                }

                tot_size = htonl(total_size);
                if(sendMessage(socket_cliente_descriptor_local, (char*) &tot_size, sizeof(tot_size)) == -1) {
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                    close(socket_cliente_descriptor_local);
                    free(user_ip_port);
                }

                snprintf(user_ip_port, total_size, "%s", publicacion->filename);

                if(sendMessage(socket_cliente_descriptor_local, user_ip_port, strlen(user_ip_port)+1) == -1) {
                    perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar el tamaño de la respuesta");
                    close(socket_cliente_descriptor_local);
                    free(user_ip_port);
                }
                free(user_ip_port);
                publicacion = publicacion->siguiente;
            }
        }
    }
    else if(peticion_local.operacion == 8){ // Función getfile
        if(respuesta < 0){
            if(sendMessage(socket_cliente_descriptor_local, (char *) &res, sizeof(res)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }  
        }
        else{
            int32_t port_cl = htonl(conexion.exist.nodo_usuario->datos_conexion->port);
            if(sendMessage(socket_cliente_descriptor_local, (char *) &port_cl, sizeof(port_cl)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }

            int32_t tot_size = htonl(strlen(conexion.exist.nodo_usuario->datos_conexion->ip)+1);
            if(sendMessage(socket_cliente_descriptor_local, (char *) &tot_size, sizeof(tot_size)) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }

            if(sendMessage(socket_cliente_descriptor_local, conexion.exist.nodo_usuario->datos_conexion->ip, strlen(conexion.exist.nodo_usuario->datos_conexion->ip)+1) == -1) {
                perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
                close(socket_cliente_descriptor_local);
            }
        }
    }
    else{
        if(sendMessage(socket_cliente_descriptor_local, (char *) &res, sizeof(res)) == -1) {
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: recibir_socket\nError: No se puede enviar la respuesta");
            close(socket_cliente_descriptor_local);
        }  
    }     
}

int main(int argc, char **argv) {
    // Comprobacion de argumentos
    if(argc != 3) {
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Número de argumentos incorrecto\n");
        return -1;
    }
    if(strcmp(argv[1], "-p") != 0) {
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Argumentos incorrectos\n");
        return -1;
    }

    // Inicializar la base de datos
    base_datos = (Lista *) malloc(sizeof(Lista));
    if (base_datos == NULL) {
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al reservar memoria para la lista de claves");
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
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al crear el socket del servidor");
		return 0;
	}
	val = 1;
	setsockopt(socket_servidor_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	// Guardar la info del socket del servidor en la estructura
    puerto_servidor = atoi(argv[2]);
    if(puerto_servidor == 0) {
        if(strcmp(argv[2], "0") != 0) {
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al obtener el puerto del servidor\n");
            return -1;
        }
    }

	bzero((char *)&socket_servidor, sizeof(socket_servidor));
	socket_servidor.sin_family      = AF_INET;
    socket_servidor.sin_addr.s_addr = INADDR_ANY;
	socket_servidor.sin_port        = htons(puerto_servidor);

    inet_ntop(AF_INET, &(socket_servidor.sin_addr), ip_servidor, INET_ADDRSTRLEN);


	// Asignar los datos del servidor al socket del servidor
	err = bind(socket_servidor_descriptor, (const struct sockaddr *)&socket_servidor, sizeof(socket_servidor));
	if (err == -1) {
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al asignar los datos del servidor al socket del servidor");
		return -1;
	}

    // Poner el socket del servidor en modo de escucha
	err = listen(socket_servidor_descriptor, SOMAXCONN);
	if (err == -1) {
        perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al poner el socket del servidor en modo de escucha");
		return -1;
	}

    // Peticiones
	pthread_attr_setdetachstate(&thread_att, PTHREAD_CREATE_DETACHED);      // threads independientes
    
    // Print inicial

    char hostname[NI_MAXHOST];
    gethostname(hostname, NI_MAXHOST);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    if (getaddrinfo(hostname, NULL, &hints, &res) == 0) {
        struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &(addr->sin_addr), ip_servidor, INET_ADDRSTRLEN);
        freeaddrinfo(res);
    } else {
        strcpy(ip_servidor, "Unknown");
    }

    printf("s> init server %s:%d\n", ip_servidor, ntohs(socket_servidor.sin_port));

    // Recibir conexiones
    while(1) {
        fflush(stdout);
        printf("s> ");
        fflush(stdout);

        // Aceptar la conexion
        socket_cliente_descriptor = accept(socket_servidor_descriptor, (struct sockaddr *)&socket_cliente, &size);
        if (socket_cliente_descriptor < 0) {
            perror("\ns> ERROR:\nFile: servidor.c\nFunction: main\nError: Error al aceptar la conexion");
            return -1;
        }

        // Info del cliente
        inet_ntop(AF_INET, &(socket_cliente.sin_addr), ip_cliente, INET_ADDRSTRLEN);
        puerto_cliente = ntohs(socket_cliente.sin_port);
        //printf("Conexión de cliente %s:%d\n", ip_cliente, puerto_cliente);

        // Recibir peticion
        //printf("\nRecibiendo peticion del cliente.\n");
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