#ifndef estructuras_h
#define estructuras_h

// Lista de usuarios
typedef struct Nodo{
    int index;
    
    char *username;
    Datos_conexion * datos_conexion;        // Estructura con los datos de la conexión actual
    Lista_publicaciones * publicaciones;    // Estructura con las publicaciones del usuario

    struct Nodo *siguiente;
}Nodo;

typedef struct Datos_conexion{
    char * ip;
    int port;
}Datos_conexion;

typedef struct Lista{
    Nodo *inicio;
    Nodo *final;
    int size;
}Lista;


// Lista de publicaciones
typedef struct Publicacion{
    int index;
    char * filename;
    char * descripcion;
}Publicacion;

typedef struct Lista_publicaciones{
    Publicacion *inicio;
    Publicacion *final;
    int size;
}Lista_publicaciones;


// Estructura de existencia
typedef struct Exist{
    int error;
    Nodo * nodo_usuario;
}Exist;

typedef struct Conexion{
    int error;
    Datos_conexion * datos_conexion;
}Conexion;

typedef struct{
    char username[256];  
    int N_value2;
    double value2[32];
    int operacion;
    /*
        0 -> destroy
        1 -> set_value
        2 -> get_value
        3 -> modify_value
        4 -> delete_key
        5 -> exist
    */
}Peticion;

#endif