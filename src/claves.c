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
    bloqueado = 1;

    // Comprobaciones
    if(sizeof(*username) > 256){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: register_user\nError: El username es demasiado largo");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }
    Exist exist;
    exist = is_registered(username);
    if(exist.error == 0){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: register_user\nError: El usuario ya existe");
        return -1;
    }

    Nodo *nodo = (Nodo *) malloc(sizeof(Nodo));
    if (nodo == NULL) {
        perror("\ns> ERROR:\nFile: claves.c\nFunction: set_value\nError: Error al reservar memoria para guardar un nodo");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }

    // Guardar los datos
    nodo->username = (char*)malloc(strlen(username)+1);
    if (nodo->username == NULL) {
        perror("\ns> ERROR:\nFile: claves.c\nFunction: set_value\nError: Error al reservar memoria para guardar el value1");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }
    strcpy(nodo->username, username);

    nodo->datos_conexion = NULL;
    nodo->publicaciones = NULL;

    if (base_datos->size == 0){ // lista vacia
        nodo->anterior = NULL;
        nodo->siguiente = NULL;
        nodo->index = 0;
        base_datos->inicio = nodo;
        base_datos->final = nodo;
        base_datos->size = 1;
    }
    else{
        nodo->anterior = base_datos->final;
        base_datos->final->siguiente = nodo;
        nodo->siguiente = NULL;
        base_datos->size++;
        nodo->index = base_datos->size;
        base_datos->final = nodo;
    }
    printf("\ns> Usuario registrado correctamente.\n");

    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para dar de baja a un usuario
int unregister(char* username){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    // Comprobaciones
    Exist exist;
    exist = is_registered(username);
    if(exist.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: unregister\nError: El usuario no existe");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    if(base_datos->size == 1){
        free(base_datos->inicio);
        base_datos->inicio = NULL;
        base_datos->final = NULL;
        base_datos->size = 0;
    }
    else{
        // Eliminar el nodo
        exist.nodo_usuario->anterior->siguiente = exist.nodo_usuario->siguiente;
        if(exist.nodo_usuario->siguiente != NULL){
            exist.nodo_usuario->siguiente->anterior = exist.nodo_usuario->anterior;
            exist.nodo_usuario->siguiente->index = exist.nodo_usuario->index;
        }
        base_datos->size--;
        free(exist.nodo_usuario);
    }
    printf("\ns> Cliente eliminado.\n");
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para conectar un usuario
int connect_user(char* username, int port, char* ip){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    // Comprobaciones
    Conexion conexion;
    conexion = is_connected(username);
    if(conexion.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: connect_user\nError: El usuario no está registrado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    else if(conexion.error == 0){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: connect_user\nError: El usuario ya está conectado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }

    Datos_conexion *datos_conexion = NULL;
    datos_conexion = (Datos_conexion *) malloc(sizeof(Datos_conexion));
    if(datos_conexion == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: connect_user\nError: Error al reservar memoria para guardar los datos de la conexion");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -3;
    }
    // Guardar los datos
    if(conexion.exist.nodo_usuario->datos_conexion == NULL){
        conexion.exist.nodo_usuario->datos_conexion = (Datos_conexion *) malloc(sizeof(Datos_conexion));
        if(conexion.exist.nodo_usuario->datos_conexion == NULL){
            perror("\ns> ERROR:\nFile: claves.c\nFunction: connect\nError: Error al reservar memoria para guardar los datos de la conexion");
            pthread_mutex_unlock(&mutex_funcion);
            bloqueado = 0;
            return -3;
        }

        datos_conexion->ip = (char *) malloc(strlen(ip) + 1);
        if (datos_conexion->ip == NULL) {
            perror("\ns> ERROR:\nFile: claves.c\nFunction: connect\nError: Error al reservar memoria para guardar la IP");
            pthread_mutex_unlock(&mutex_funcion);
            bloqueado = 0;
            return -3;
        }

        memcpy(conexion.exist.nodo_usuario->datos_conexion, datos_conexion, sizeof(Datos_conexion));
        conexion.exist.nodo_usuario->datos_conexion->port = port;
        conexion.exist.nodo_usuario->datos_conexion->ip = strdup(ip);
    }

    printf("\ns> Usuario conectado.\n");
    free(datos_conexion->ip);
    free(datos_conexion);
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para desconectar un usuario
int disconnect(char* username){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    // Comprobaciones

    Conexion conexion;
    conexion = is_connected(username);
    if(conexion.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: disconnect\nError: El usuario no está registrado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    else if(conexion.error == -2){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: disconnect\nError: El usuario no está conectado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }

    // Eliminar los datos
    free(conexion.exist.nodo_usuario->datos_conexion);
    conexion.exist.nodo_usuario->datos_conexion = NULL;

    printf("\ns> Usuario desconectado.\n");
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para publicar un fichero
int publish(char * username_publisher, char* file_name, char* descripcion){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    // Comprobaciones
    if(strlen(file_name) > 256){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El nombre del fichero es demasiado largo");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }

    Conexion conexion;
    conexion = is_connected(username_publisher);
    if(conexion.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El usuario no está registrado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    else if(conexion.error == -2){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El usuario no está conectado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }

    // Guardar el fichero
    Lista_publicaciones * publicaciones = NULL;
    publicaciones = (Lista_publicaciones *) malloc(sizeof(Lista_publicaciones));
    if(publicaciones == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: Error al reservar memoria para guardar la lista de publicaciones");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }

    Publicacion * publicacion = NULL;
    publicacion = (Publicacion *) malloc(sizeof(Publicacion));
    if(publicacion == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: Error al reservar memoria para guardar la publicacion");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }

    publicacion->filename = strdup(file_name);
    publicacion->descripcion = strdup(descripcion);

    if(conexion.exist.nodo_usuario->publicaciones == NULL){
        publicaciones->inicio = publicacion;
        publicaciones->final = publicacion;
        publicaciones->size = 1;
        conexion.exist.nodo_usuario->publicaciones = publicaciones;
    }
    else{
        publicaciones = conexion.exist.nodo_usuario->publicaciones;
        Published published = is_published(file_name, publicaciones);
        if(published.error == -1){
            publicacion->index = publicaciones->size;
            publicacion->anterior = publicaciones->final;
            publicacion->siguiente = NULL;

            publicaciones->final = publicacion;
            publicaciones->size++;
        }
        else{
            perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El fichero ya está publicado");
            pthread_mutex_unlock(&mutex_funcion);
            bloqueado = 0;
            return -3;
        }
        
    }

    printf("\ns> Fichero publicado.\n");
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
    return 0;
}

// Funcion para borrar un fichero
int delete(char * username_solicitante, char * file_name){
    pthread_mutex_lock(&mutex_funcion);
    bloqueado = 1;

    // Comprobaciones
    Conexion conexion;
    conexion = is_connected(username_solicitante);
    if(conexion.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El usuario no está registrado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -1;
    }
    else if(conexion.error == -2){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El usuario no está conectado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -2;
    }

    Published published;
    published = is_published(file_name,conexion.exist.nodo_usuario->publicaciones);
    if(published.error == -1){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: publish\nError: El fichero no esta publicado");
        pthread_mutex_unlock(&mutex_funcion);
        bloqueado = 0;
        return -3;
    }

    // Eliminar el fichero
    if(published.publicacion->anterior == NULL){
        conexion.exist.nodo_usuario->publicaciones->inicio = published.publicacion->siguiente;
    }
    else{
        published.publicacion->anterior->siguiente = published.publicacion->siguiente;
    }

    if(published.publicacion->siguiente != NULL){
        published.publicacion->siguiente->anterior = published.publicacion->anterior;
        published.publicacion->siguiente->index = published.publicacion->index;
    }
    conexion.exist.nodo_usuario->publicaciones->size--;
    free(published.publicacion);

    printf("\ns> Fichero borrado.\n");
    pthread_mutex_unlock(&mutex_funcion);
    bloqueado = 0;
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
        perror("\ns> ERROR:\nFile: claves.c\nFunction: is_registered\nError: La lista esta vacia");
        if(bloqueado == 0){
            pthread_mutex_unlock(&mutex_funcion);
        }
        exist.error = -1;
        return exist;
    }
    else{
        Nodo *nodo_actual = base_datos->inicio;
        for(int indice = 0; indice < base_datos->size; indice++){
            if(strcmp(nodo_actual->username, username) == 0){
                printf("\ns> Usuario encontrado.\n");
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
        bloqueado = 2;
    }
    if (base_datos == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: is_connected\nError: La lista esta vacia");
        if(bloqueado == 0 || bloqueado == 2){
            pthread_mutex_unlock(&mutex_funcion);
        }
        conexion.error = -1;
        return conexion;
    }
    else{
        exist = is_registered(username);
        if(exist.error == -1){
            perror("\ns> ERROR:\nFile: claves.c\nFunction: is_connected\nError: El usuario no está registrado"); 
            if(bloqueado == 0 || bloqueado == 2){
                pthread_mutex_unlock(&mutex_funcion);
            }
            conexion.error = -1;
            conexion.exist = exist;
            return conexion;
        }
        else{
            if(exist.nodo_usuario->datos_conexion == NULL){
                perror("\ns> ERROR:\nFile: claves.c\nFunction: is_connected\nError: El usuario no está conectado"); 
                if(bloqueado == 0 || bloqueado == 2){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                conexion.error = -2;
                conexion.exist = exist;
                return conexion;
            }
            else{
                if (bloqueado == 0 || bloqueado == 2){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                conexion.error = 0;
                conexion.exist = exist;
                conexion.datos_conexion = exist.nodo_usuario->datos_conexion;
                return conexion;
            }
        }
    }
}

// Comprobar si un fichero esta publicado
Published is_published(char * file_name, Lista_publicaciones * publicaciones){
    Published publicacion;

    publicacion.publicacion = malloc(sizeof(Publicacion));
    if(publicacion.publicacion == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: is_published\nError: No se ha podido reservar memoria");
        publicacion.error = -1;
        return publicacion;
    }

    if (bloqueado == 0){
        pthread_mutex_lock(&mutex_funcion);
    }

    if(publicaciones == NULL){
        perror("\ns> ERROR:\nFile: claves.c\nFunction: is_published\nError: La lista esta vacia");
        if(bloqueado == 0){
            pthread_mutex_unlock(&mutex_funcion);
        }
        publicacion.error = -1;
        return publicacion;
    }

    Publicacion *publicacion_actual = publicaciones->inicio;
    for(int indice = 0; indice < publicaciones->size; indice++){
        if(strcmp(publicacion_actual->filename, file_name) == 0){
            printf("\ns> Publicacion encontrada.\n");
            if (bloqueado == 0){
                pthread_mutex_unlock(&mutex_funcion);
            }
            publicacion.error = 0;
            memcpy(publicacion.publicacion, publicacion_actual, sizeof(Publicacion));
            return publicacion;
        }
        else{
            if(publicacion_actual != publicaciones->final){
                publicacion_actual = publicacion_actual->siguiente;
            }
            else{
                if(bloqueado == 0){
                    pthread_mutex_unlock(&mutex_funcion);
                }
                publicacion.error = -1;
                return publicacion;
            }
        }
    }
    if (bloqueado == 0){
        pthread_mutex_unlock(&mutex_funcion);
    }
    publicacion.error = -1;
    return publicacion;
}