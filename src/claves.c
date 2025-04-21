#include "estructuras.h"
#include "claves.h"
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Variable global
extern Lista *base_datos;
extern pthread_mutex_t mutex_funcion;
int bloqueado = 0;

// Funciones principales
// Funcion para registrar un nuevo usuario
int register_user(char* username){
    pthread_mutex_lock(&mutex_funcion);

    Exist exist;
    exist = is_registered(username);
    if(exist.error == 0){
        perror("ERROR:\nFile: claves.c\nFunction: register_user\nError: El usuario ya existe");
        return -1;
    }
    
    Nodo *nodo = (Nodo *) malloc(sizeof(Nodo));
    if (nodo == NULL) {
        perror("ERROR:\nFile: claves.c\nFunction: set_value\nError: Error al reservar memoria para guardar un nodo");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }

    nodo->username = (char*)malloc(strlen(username)+1);
    if (nodo->username == NULL) {
        perror("ERROR:\nFile: claves.c\nFunction: set_value\nError: Error al reservar memoria para guardar el value1");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    strcpy(nodo->username, username);

    nodo->datos_conexion = NULL;
    nodo->publicaciones = NULL;

    if (base_datos->size == 0){ // lista vacia
        nodo->siguiente = NULL;
        nodo->index = 0;

        base_datos = (Lista *) malloc(sizeof(Lista));
        if (base_datos == NULL) {
            perror("ERROR:\nFile: claves.c\nFunction: register_user\nError: Error al reservar memoria para la base de datos");
            pthread_mutex_unlock(&mutex_funcion);
            bloqueado = 0;
            return -1;
        }

        base_datos->inicio = nodo;
        base_datos->final = nodo;
        base_datos->size = 1;
    }
    else{
        nodo->siguiente = NULL;
        nodo->index = base_datos->size;
        base_datos->final = nodo;
        base_datos->final->siguiente = NULL;
        base_datos->size++;
    }
    printf("Mensaje guardado en la lista.\n");

    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para dar de baja a un usuario
int unregister(char* username){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    Exist exist;
    exist = is_registered(username);
    if(exist.error == 0){
        perror("ERROR:\nFile: claves.c\nFunction: register_user\nError: El usuario ya existe");
        return -1;
    }
    if(base_datos->size == 1){
        free(base_datos->inicio);
        base_datos->inicio = NULL;
        base_datos->final = NULL;
        base_datos->size = 0;
    }
    else{
        Nodo *nodo_anterior = base_datos->inicio;
        for(int indice = 0; indice < base_datos->size; indice++){
            if(exist.nodo_usuario == base_datos->inicio){
                base_datos->inicio = exist.nodo_usuario->siguiente;
            }
            else if(exist.nodo_usuario == base_datos->final){
                base_datos->final = nodo_anterior;
            }
            else if (nodo_anterior->siguiente == exist.nodo_usuario){
                nodo_anterior->siguiente = exist.nodo_usuario->siguiente;
            }
            base_datos->size--;
            free(exist.nodo_usuario);
        }
    }
    printf("Datos asociados a la clave destruidos.\n");
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para conectar un usuario
int connect(char* username){
    return 0;
}

// Funcion para desconectar un usuario
int disconnect(char* username){
    return 0;
}

// Funcion para publicar un fichero
int publish(char * username_publisher, char* username, char* file_name){
    return 0;
}

// Funcion para borrar un fichero
int delete(char * username_solicitante, char * file_name){
    return 0;
}

// Funcion para mostrar la lista de usuarios conectados
int listusers(char * username_solicitante){
    return 0;
}

// Funcion para mostrar la lista de contenidos publicados por un usuario
int listcontent(char * username_solicitante, char * username){
    return 0;
}

// Funcion para descargar un fichero
int getfile(char * username_solicitante, char *  username, char * remote_file_mame, char * local_file_mame){
    return 0;
}



// Funciones Auxiliares
// Comprobar si el usuario esta registrado
Exist is_registered(char * username){
    Exist exist;

    if (bloqueado == 0){
        pthread_mutex_lock(&mutex_funcion);
    }

    if (base_datos == NULL){
        perror("ERROR:\nFile: claves.c\nFunction: is_registered\nError: La lista esta vacia");
        if(bloqueado == 0){
            pthread_mutex_unlock(&mutex_funcion);
        }
        exist.error = -1;
        return exist;
    }
    else{
        Nodo *nodo_actual = base_datos->inicio;
        for(int indice = 0; indice < base_datos->size; indice++){
            if(strcmp(nodo_actual->username, *username == 0)){
                printf("Usuario encontrad0.\n");
                if (bloqueado == 0){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                exist.error = 0;
                exist.nodo_usuario = nodo_actual;
                return exist;
            }
            else{
                if(nodo_actual != base_datos->final){
                    nodo_actual = nodo_actual->siguiente;
                }
                else{
                    if(bloqueado == 0){
                        pthread_mutex_unlock(&mutex_funcion);
                    }
                    exist.error = -1;
                    return exist;
                }
            }
        }
        if (bloqueado == 0){
            pthread_mutex_unlock(&mutex_funcion);
        }
        exist.error = -1;
        return exist;
    }
}

// Comprobar si el usuario está conectado
Conexion is_connected(char * username){
    Exist exist;
    Conexion conexion;

    if (bloqueado == 0){
        pthread_mutex_lock(&mutex_funcion);
    }
    if (base_datos == NULL){
        perror("ERROR:\nFile: claves.c\nFunction: is_connected\nError: La lista esta vacia");
        if(bloqueado == 0){
            pthread_mutex_unlock(&mutex_funcion);
        }
        conexion.error = -1;
        return conexion;
    }
    else{
        exist = is_registered(username);
        if(exist.error == -1){
            perror("ERROR:\nFile: claves.c\nFunction: is_connected\nError: El usuario no esta registrado"); 
            if(bloqueado == 0){
                pthread_mutex_unlock(&mutex_funcion);
            }
            conexion.error = -1;
            return conexion;
        }
        else{
            if(exist.nodo_usuario->datos_conexion == NULL){
                perror("ERROR:\nFile: claves.c\nFunction: is_connected\nError: El usuario no esta conectado");
                if(bloqueado == 0){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                conexion.error = -1;
                return conexion;
            }
            else{
                if (bloqueado == 0){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                conexion.error = 0;
                conexion.datos_conexion = exist.nodo_usuario->datos_conexion;
                return conexion;
            }
        }
    }
}