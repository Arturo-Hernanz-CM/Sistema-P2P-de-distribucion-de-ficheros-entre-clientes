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

// Hilo del cliente
pthread_t thread_cliente;
pthread_attr_t thread_att_cliente;
pthread_mutex_t mutex_peticion;
pthread_cond_t condicion_peticion;
int ocupado = 0;
struct sockaddr_in socket_cliente_escucha, socket_cliente;
socklen_t size;
int puerto_escucha;
volatile int hilo_activo = 1;
int socket_escucha_descriptor;

// Funciones

// Enviar peticion
int enviar_peticion(int socket_cliente_descriptor, Peticion * peticion){
   int32_t operacion_, port_;

   operacion_ = htonl(peticion->operacion);
   port_ = htonl(peticion->port_escucha);

   // Enviar operacion
    if (sendMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Enviar puerto
    if (sendMessage(socket_cliente_descriptor, (char *) &port_, sizeof(port_)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar el puerto\n");
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

    // Enviar fechahora
    if (sendMessage(socket_cliente_descriptor, peticion->fecha_hora, sizeof(peticion->fecha_hora)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: enviar_peticion\nError: Error al enviar la fechahora\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    return 0;
}

// Recibir peticion
int recibir_peticion(int socket_cliente_descriptor, Peticion * resp){
    int32_t operacion_, port_;

    // Recibir operacion
    if(recvMessage(socket_cliente_descriptor, (char *) &operacion_, sizeof(operacion_)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir la operacion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir puerto
    if(recvMessage(socket_cliente_descriptor, (char *) &port_, sizeof(port_)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir el puerto\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir username
    if(recvMessage(socket_cliente_descriptor, resp->username, sizeof(resp->username)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir username\n");
        close(socket_cliente_descriptor);
        return -1;
    }
    
    // Recibir username2
    if(recvMessage(socket_cliente_descriptor, resp->username2, sizeof(resp->username2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir username2\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename1
    if(recvMessage(socket_cliente_descriptor, resp->filename1, sizeof(resp->filename1)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir filename1\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir descripcion    
    if(recvMessage(socket_cliente_descriptor, resp->description, sizeof(resp->description)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir la descripcion\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Recibir filename2
    if(recvMessage(socket_cliente_descriptor, resp->filename2, sizeof(resp->filename2)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir filename2\n");
        close(socket_cliente_descriptor);
        return -1;
    }    

    // Recibir fechahora
    if(recvMessage(socket_cliente_descriptor, resp->fecha_hora, sizeof(resp->fecha_hora)) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_peticion\nError: Error al recibir la fechahora\n");
        close(socket_cliente_descriptor);
        return -1;
    }

    // Asignacion de valores a la estructura
    resp->operacion = ntohl(operacion_);
    resp->port_escucha = ntohs(port_);

    return 0;
}

// Comunicacion peer-to-peer
// Función del hilo del cliente
void recibir_cliente(int* socket_cliente_descriptor){
    // Variables
    int socket_cliente_descriptor_local;
    Peticion peticion_local = {0};
    int respuesta;
    int32_t res;
    char buffer[1024];
    ssize_t bytes_leidos = 0;
    FILE * descriptor_fichero;

    // Copiar el descriptor del socket del cliente
    pthread_mutex_lock(&mutex_peticion);
    socket_cliente_descriptor_local = (*(int *) socket_cliente_descriptor);
    ocupado = 1;
    pthread_cond_signal(&condicion_peticion);
    pthread_mutex_unlock(&mutex_peticion);

    // Recibe los datos del cliente
    if (recibir_peticion(socket_cliente_descriptor_local, &peticion_local) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Error al recibir la petición\n");
        close(socket_cliente_descriptor_local);
        respuesta = -1;
    }

    // Ejecutar la petición
    if(peticion_local.operacion == 8){
        descriptor_fichero = fopen(peticion_local.filename1, "r");
        if(descriptor_fichero == NULL){
            perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Error al abrir el fichero\n");
            close(socket_cliente_descriptor_local);
            respuesta = -10;
        }
    }
    else{
        perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Peticion no soportada\n");
        close(socket_cliente_descriptor_local);
        respuesta = -1;
    }

    // Enviar la respuesta
    printf("Enviando respuesta a cliente: %d\n", socket_cliente_descriptor_local);

    res = htonl(respuesta);
    if(respuesta < 0){
        if(sendMessage(socket_cliente_descriptor_local, (char *) &res, sizeof(res)) == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Error al enviar la respuesta\n");
            close(socket_cliente_descriptor_local);
        }
    }
    if(respuesta >= 0){
        // Calcular el tamaño del fichero
        fseek(descriptor_fichero, 0L, SEEK_END); 
        long int tam = ftell(descriptor_fichero);
        char size_file[32];
        snprintf(size_file,sizeof(size_file), "%ld", tam); 
        if(sendMessage(socket_cliente_descriptor_local, size_file, sizeof(size_file)) == -1){
            perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Error al enviar la respuesta\n");
            close(socket_cliente_descriptor_local);
        }

        // Enviar el contenido del fichero
        rewind(descriptor_fichero);
        bytes_leidos = fread(buffer, 1, sizeof(buffer), descriptor_fichero);
        while(bytes_leidos > 0){
            if(sendMessage(socket_cliente_descriptor_local, buffer, bytes_leidos) == -1){
                perror("ERROR:\nFile: proxy.c\nFunction: recibir_cliente\nError: Error al enviar la respuesta\n");
                close(socket_cliente_descriptor_local);
            }
            bytes_leidos = fread(buffer, 1, sizeof(buffer), descriptor_fichero);
        }
        fclose(descriptor_fichero);
    }

}

// Funcion para enviar peticiones a otros clientes
int comunicacion_cliente(Peticion * peticion, char * ip_dest, int port_dest){
    // Variables
    int socket_cliente_origen;
	struct sockaddr_in socket_cliente_destino;	// socket del servidor
	struct hostent *datos_cliente_destino;	// datos del cliente_destino
	int error;
    FILE *fichero_local;
    char buffer[1024];
    
    // Socket del cliente
	socket_cliente_origen = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_cliente_origen == -1) {
		perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: Error al crear el socket del cliente\n");
        return -1;
	}

    // Socket del cliente desrtino
	bzero((char *)&socket_cliente_destino, sizeof(socket_cliente_destino));
   	datos_cliente_destino = gethostbyname(ip_dest);
	if (datos_cliente_destino == NULL) {
		perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: Error al obtener la informacion del cliente_destino\n");
		close(socket_cliente_origen);
        return -1;
	}

    // Guardar los datos del cliente_destino en la estructura para usarlos en el socket
    memcpy (&(socket_cliente_destino.sin_addr), datos_cliente_destino->h_addr_list[0], datos_cliente_destino->h_length);
    socket_cliente_destino.sin_family  = AF_INET;
    socket_cliente_destino.sin_port    = htons(port_dest);
    
    // Conectar el socket del cliente con el cliente_destino
    error = connect(socket_cliente_origen, (struct sockaddr *) &socket_cliente_destino,  sizeof(socket_cliente_destino));
	if (error == -1) {
		perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: Error al conectar el socket del cliente con el cliente_destino\n");
		close(socket_cliente_origen);
        return -1;
	}

    // Enviar datos al cliente_destino
    printf("\nEnviando peticion.\n");
    if (enviar_peticion(socket_cliente_origen, peticion) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: Error al enviar la peticion\n");
        close(socket_cliente_origen);
        return -1;
    }

    char size_file[32];

    if(recvMessage(socket_cliente_origen, size_file, sizeof(size_file)) == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: No se puede recibir la respuesta");
        close(socket_cliente_origen);
        return -1;
    }

    char *endptr;
    long int fin_emit = strtol(size_file, &endptr, 10);
    
    if (*endptr != '\0') {
        fprintf(stderr, "ERROR: El valor recibido no es un número válido\n");
        close(socket_cliente_origen);
        return -1;
    }

    fichero_local = fopen(peticion->filename2, "w");
    if(fichero_local == NULL){
        perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: Error al abrir el fichero local\n");
        close(socket_cliente_origen);
        return -1;
    }

    while (fin_emit > 0) {
        int bytes_a_recibir = (fin_emit < sizeof(buffer)) ? fin_emit : sizeof(buffer);

        if (recvMessage(socket_cliente_origen, buffer, bytes_a_recibir) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_cliente\nError: No se puede recibir el contenido del fichero");
            close(socket_cliente_origen);
            fclose(fichero_local);
            return -1;
        }
        printf("Recibiendo %ld bytes\n", fin_emit);
    
        fwrite(buffer, 1, bytes_a_recibir, fichero_local);
        fin_emit -= bytes_a_recibir;
    }
    fclose(fichero_local);

    // Cerrar socket
    close(socket_cliente_origen);
    return 0;
}

// Crear el hilo del cliente
void* escuchar_socket_cliente(void * arg){
    int puerto_cliente;
    char ip_cliente[INET_ADDRSTRLEN];

    // Estructuras para los sockets
    struct sockaddr_in socket_cliente_escucha, socket_cliente_;
    socklen_t size;
    socklen_t len = sizeof(socket_cliente_escucha);

    socket_escucha_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    int socket_cliente_descriptor_;

    if (socket_escucha_descriptor == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: escuchar_socket_cliente\nError: Error al crear el socket\n");
        close(socket_escucha_descriptor);
    }

    int val = 1;
	setsockopt(socket_escucha_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	// Guardar la info del socket de escucha en la estructura
	bzero((char *)&socket_cliente_escucha, sizeof(socket_cliente_escucha));
	socket_cliente_escucha.sin_family      = AF_INET;
    socket_cliente_escucha.sin_addr.s_addr = INADDR_ANY;
	socket_cliente_escucha.sin_port        = htons(0);

	// Asignar los datos del servidor al socket del servidor
	int err = bind(socket_escucha_descriptor, (const struct sockaddr *)&socket_cliente_escucha, sizeof(socket_cliente_escucha));
	if (err == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: escuchar_socket_cliente\nError: Error al asignar los datos del cliente al socket de escucha");
        close(socket_escucha_descriptor);
	}

    // Escuchar el socket
    err = listen(socket_escucha_descriptor, 5);
    if (err == -1) {
        perror("ERROR:\nFile: proxy.c\nFunction: escuchar_socket_cliente\nError: Error al escuchar el socket");
        close(socket_escucha_descriptor);
    }

    // Obtener el puerto dinamicamente
    if (getsockname(socket_escucha_descriptor, (struct sockaddr *)&socket_cliente_escucha, &len) == -1) {
        perror("getsockname");
    } else {
        puerto_escucha = ntohs(socket_cliente_escucha.sin_port);
        printf("Puerto de escucha P2P: %d\n", puerto_escucha);
    }

    // Hilo
    pthread_t thread_cliente_cliente;
    pthread_attr_t thread_att_cliente_cliente;


    // Inicializar mutex
    pthread_attr_init(&thread_att_cliente_cliente);
    pthread_mutex_init(&mutex_peticion, NULL);
    pthread_cond_init(&condicion_peticion, NULL);

    pthread_attr_setdetachstate(&thread_att_cliente_cliente, PTHREAD_CREATE_DETACHED);      // threads independientes
    while(hilo_activo == 1) {
        // Aceptar la conexion
        size = sizeof(socket_cliente_);
        socket_cliente_descriptor_ = accept(socket_escucha_descriptor, (struct sockaddr *)&socket_cliente_, &size);
        if (socket_cliente_descriptor_ < 0) {
            perror("ERROR:\nFile: proxy.c\nFunction: escuchar_socket_cliente\nError: Error al aceptar la conexion");
            close(socket_escucha_descriptor);
        }

        // Info del cliente
        inet_ntop(AF_INET, &(socket_cliente_.sin_addr), ip_cliente, INET_ADDRSTRLEN);
        puerto_cliente = ntohs(socket_cliente_.sin_port);
        printf("Conexión de cliente %s:%d\n", ip_cliente, puerto_cliente);

        // Recibir peticion
        printf("\nRecibiendo peticion del cliente.\n");
        if(pthread_create(&thread_cliente_cliente, &thread_att_cliente_cliente, (void*) recibir_cliente, &socket_cliente_descriptor_) == 0) {
            pthread_mutex_lock(&mutex_peticion);
            while(ocupado == 0) {
                pthread_cond_wait(&condicion_peticion, &mutex_peticion);
            }
            ocupado = 0;
            pthread_mutex_unlock(&mutex_peticion);
        }
    }
    close(socket_escucha_descriptor);
    pthread_exit(NULL);
}

// Comunicacion con el servidor
// Funcion para comunicarse con el servidor
int comunicacion_servidor(Peticion * peticion, char ** ip_dest, int ** port_dest) {
    // Variables
    int puerto_servidor;
    int error = 0;
    if(peticion->operacion == 0 || peticion->operacion == 1){
        error = -2;
    }
    else if(peticion->operacion == 2 || peticion->operacion == 3 || peticion->operacion == 6){
        error = -3;
    }
    else if(peticion->operacion == 4 || peticion->operacion == 5 || peticion->operacion == 7){
        error = -4;
    }
    else if(peticion->operacion == 8){
        error = -1;
    }
    
    // Comprobacion de las variables
    if(puerto_servidor_char == NULL){
        perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al obtener el puerto del servidor\n");
        return -1;
    }
    puerto_servidor = atoi(puerto_servidor_char);
    if(puerto_servidor == 0){
        if (strcmp(puerto_servidor_char, "0") != 0) {
            perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al obtener el puerto del servidor\n");
            return error;
        }
    }

    // Variables
    int respuesta;
    int socket_cliente_descriptor;
	struct sockaddr_in socket_servidor;	// socket del servidor
	struct hostent *datos_servidor;	// datos del servidor
	int32_t res;
	int error_;
    
    // Socket del cliente
	socket_cliente_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_cliente_descriptor == -1) {
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al crear el socket del cliente\n");
        return error;
	}

    // Socket del servidor
	bzero((char *)&socket_servidor, sizeof(socket_servidor));
   	datos_servidor = gethostbyname(ip_servidor);
	if (datos_servidor == NULL) {
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al obtener la informacion del servidor\n");
		close(socket_cliente_descriptor);
        return error;
	}

    // Guardar los datos del servidor en la estructura para usarlos en el socket
    memcpy (&(socket_servidor.sin_addr), datos_servidor->h_addr_list[0], datos_servidor->h_length);
    socket_servidor.sin_family  = AF_INET;
    socket_servidor.sin_port    = htons(puerto_servidor);
    
    // Conectar el socket del cliente con el servidor
    error_ = connect(socket_cliente_descriptor, (struct sockaddr *) &socket_servidor,  sizeof(socket_servidor));
	if (error_ == -1) {
		perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al conectar el socket del cliente con el servidor\n");
		close(socket_cliente_descriptor);
        return error;
	}

    // Enviar datos al servidor
    printf("Enviando peticion.\n");
    if (enviar_peticion(socket_cliente_descriptor, peticion) == -1){
        perror("ERROR:\nFile: proxy.c\nFunction: tratar_peticion\nError: Error al enviar la peticion\n");
        close(socket_cliente_descriptor);
        return error;
    }

    if(peticion->operacion == 6 || peticion->operacion == 7){
        int fin_emit;
        int32_t fin_emit_, tot_size;
        if(recvMessage(socket_cliente_descriptor, (char *) &fin_emit_, sizeof(fin_emit_)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }
        fin_emit = ntohl(fin_emit_);

        char* buffer = NULL;

        printf("Recibiendo respuesta del servidor.\n");
        if(peticion->operacion == 6){
            printf("Usuarios conectados al sistema:\n");
        }
        else if(peticion->operacion == 7){
            if(fin_emit == 0){
                printf("El usuario %s no tiene publicaciones\n", peticion->username);
            }
            else{
                printf("Publicaciones del usuario %s:\n", peticion->username);
            }
        }
        for(int i = 0; i < fin_emit; i++){

            if(recvMessage(socket_cliente_descriptor, (char *) &tot_size, sizeof(int32_t)) == -1) {
                perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
                close(socket_cliente_descriptor);
                return error;
            }
            tot_size = ntohl(tot_size);

            buffer = (char*)malloc( sizeof(char)*tot_size);
            if(buffer == NULL){
                perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
                close(socket_cliente_descriptor);
                return error;
            }

            if(recvMessage(socket_cliente_descriptor, buffer, tot_size) == -1) {
                perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
                close(socket_cliente_descriptor);
                return error;
            }
            printf("%s\n", buffer);
            free(buffer);
        }
        close(socket_cliente_descriptor);
        
    }
    else if(peticion->operacion == 8){
        printf("Recibiendo respuesta del servidor.\n");
        if(recvMessage(socket_cliente_descriptor, (char *) &res, sizeof(int32_t)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }

        int puerto = ntohl(res);
        if(puerto < 0){
            return error;
        }
        *port_dest = (int*) malloc(sizeof(int));
        if(*port_dest == NULL){
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }
        **port_dest = puerto;

        if(recvMessage(socket_cliente_descriptor, (char *) &res, sizeof(int32_t)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }

        int total_size = ntohl(res);
        *ip_dest = (char*) malloc(sizeof(char)*total_size);
        if(*ip_dest == NULL){
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }
        if(recvMessage(socket_cliente_descriptor, *ip_dest, total_size) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }
        close(socket_cliente_descriptor);

    }
    else{
        printf("Recibiendo respuesta del servidor.\n");
        if(recvMessage(socket_cliente_descriptor, (char *) &res, sizeof(int32_t)) == -1) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: No se puede recibir la respuesta");
            close(socket_cliente_descriptor);
            return error;
        }
        respuesta = ntohl(res);
        if (respuesta < 0) {
            perror("ERROR:\nFile: proxy.c\nFunction: comunicacion_servidor\nError: Error en la respuesta");
            close(socket_cliente_descriptor);
            if(respuesta < error){
                return error;
            }
            else{
                return respuesta;
            }
        }
    }

    // Cerrar socket
    close(socket_cliente_descriptor);
    return 0;
}

// Funciones principales
// Funcion para registrar un nuevo usuario
int register_user(char* username, char* ip_s, char* port_s, char * fecha_hora) {
    // Crear el hilo para escuchar peticiones de otros usuarios si no está creado
    if(hilo == 0){
        // Inicializar hilos
        pthread_attr_init(&thread_att_cliente);

        if(pthread_create(&thread_cliente,&thread_att_cliente, escuchar_socket_cliente, NULL) != 0){
            perror("ERROR:\nFile: proxy.c\nFunction: register_user\nError: Error al crear el hilo");
            return -1;
        }
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
    
    strcpy(peticion.fecha_hora, fecha_hora);
    strcpy(peticion.username, username);
    peticion.operacion = 0;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("USERNAME IN USE\n\n");
        free(ip_servidor);
        return -1;
    }
    else if(resultado <= -2){
        perror("REGISTER FAIL\n\n");
        free(ip_servidor);
        return -1;
    }
    printf("REGISTER OK\n\n");
    return 0;
}

// Funcion para dar de baja a un usuario
int unregister(char* username, char * fecha_hora) {
    strcpy(peticion.username, username);
    strcpy(peticion.fecha_hora, fecha_hora);
    peticion.operacion = 1;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado <= -2){
        perror("UNREGISTER FAIL\n\n");
        return -1;
    }
    if(hilo == 1){
        hilo_activo = 0;
        shutdown(socket_escucha_descriptor, SHUT_RDWR);  // Interrumpe accept() del hilo de escuchas
        close(socket_escucha_descriptor);
        pthread_join(thread_cliente, NULL);
        hilo = 0;
    }

    printf("UNREGISTER OK\n\n");
    return 0;
}

// Funcion para conectar al cliente al sistema
int connect_user(char* username, char * fecha_hora) {
    strcpy(peticion.username, username);
    strcpy(peticion.fecha_hora, fecha_hora);
    peticion.operacion = 2;
    peticion.port_escucha = puerto_escucha;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("CONNECT FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("USER ALREADY CONNECTED\n\n");
        return -1;
    }
    else if(resultado <= -3){
        perror("CONNECT FAIL\n\n");
        return -1;
    }

    printf("CONNECT OK\n\n");
    return 0;
}

// Funcion para desconectar al cliente del sistema
int disconnect(char* username, char * fecha_hora) {
    strcpy(peticion.username, username);
    strcpy(peticion.fecha_hora, fecha_hora);
    peticion.operacion = 3;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("DISCONNECT FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("DISCONNECT FAIL, USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado <= -3){
        perror("DISCONNECT FAIL\n\n");
        return -1;
    }

    printf("DISCONNECT OK\n\n");
    return 0;
}

// Funcion para publicar un fichero
int publish(char * username_publisher, char* file_name, char* descripcion, char * fecha_hora) {
    strcpy(peticion.username, username_publisher);
    strcpy(peticion.filename1, file_name);
    strcpy(peticion.description, descripcion);
    strcpy(peticion.fecha_hora, fecha_hora);
    peticion.operacion = 4;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("PUBLISH FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("PUBLISH FAIL, USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado == -3){
        perror("PUBLISH FAIL, CONTENT ALREADY PUBLISHED\n\n");
        return -1;
    }
    else if(resultado <= -4){
        perror("PUBLISH FAIL\n\n");
        return -1;
    }

    printf("PUBLISH OK\n\n");
    return 0;
}

// Funcion para borrar un fichero
int delete(char * username_solicitante, char * file_name, char * fecha_hora) {
    strcpy(peticion.username, username_solicitante);
    strcpy(peticion.filename1, file_name);
    strcpy(peticion.fecha_hora, fecha_hora);
    peticion.operacion = 5;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("DELETE FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("DELETE FAIL, USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado == -3){
        perror("DELETE FAIL, CONTENT NOT PUBLISHED\n\n");
        return -1;
    }
    else if(resultado <= -4){
        perror("DELETE FAIL\n\n");
        return -1;
    }

    printf("DELETE OK\n\n");
    return 0;
}

// Funcion para obtener los usuarios
int listusers(char * username, char * fecha_hora) {
    strcpy(peticion.username, username);
    peticion.operacion = 6;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("LISTUSERS FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("LISTUSERS FAIL, USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado <= -3){
        perror("LISTUSERS FAIL\n\n");
        return -1;
    }

    printf("LISTUSERS OK\n\n");
    return 0;
}

// Funcion para obtener los ficheros
int listcontent(char * username, char * username_solicitante, char * fecha_hora) {
    strcpy(peticion.username, username);
    strcpy(peticion.username2, username_solicitante);
    peticion.operacion = 7;

    int resultado = comunicacion_servidor(&peticion, NULL, NULL);
    if(resultado == -1){
        perror("LISTCONTENT FAIL, USER DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado == -2){
        perror("LISTCONTENT FAIL, USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado == -3){
        perror("LISTCONTENT FAIL, REMOTE USER NOT CONNECTED\n\n");
        return -1;
    }
    else if(resultado <= -4){
        perror("LISTCONTENT FAIL\n\n");
        return -1;
    }

    printf("LISTCONTENT OK\n\n");
    return 0;
}

// Funcion para obtener un fichero
int get_file(char * username, char * username_solicitante, char * remote_filename, char * local_filename, char * fecha_hora) {
    strcpy(peticion.username, username);
    strcpy(peticion.username2, username_solicitante);
    strcpy(peticion.filename1, remote_filename);
    strcpy(peticion.filename2, local_filename);
    peticion.operacion = 8;

    // Peticion para obterner el puerto e ip del cliente con el fichero al servidor
    char * ip_dest = NULL;
    int * port_dest = NULL;

    int resultado_servidor = comunicacion_servidor(&peticion, &ip_dest, &port_dest);
    if (resultado_servidor < 0){
        perror("GETFILE FAIL\n\n");
        return -1;
    }

    int resultado = comunicacion_cliente(&peticion, ip_dest, *port_dest);

    free(ip_dest);
    free(port_dest);

    if(resultado == -10){
        perror("GETFILE FAIL, FILE DOES NOT EXIST\n\n");
        return -1;
    }
    else if(resultado <= -1){
        perror("GETFILE FAIL\n\n");
        return -1;
    }

    printf("GETFILE OK\n\n");
    return 0;
}

// Funcion para salir del proxy
int quit(char * username, char * fecha_hora) {
    int disconnect_result = disconnect(username, fecha_hora);
    if(disconnect_result == -1){
        return -1;
    }

    free(ip_servidor);
    free(puerto_servidor_char);
    return 0;
}