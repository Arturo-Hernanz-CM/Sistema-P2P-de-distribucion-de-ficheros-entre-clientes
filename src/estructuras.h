#ifndef estructuras_h
#define estructuras_h

// Estructuras auxiliares

// Datos de las conexiones
typedef struct Datos_conexion{
    char * ip;
    char * port;
}Datos_conexion;

// Lista de publicaciones
typedef struct Publicacion{
    int index;
    char * filename;
    char * descripcion;

    struct Publicacion *anterior;
    struct Publicacion *siguiente;
} Publicacion;

typedef struct Lista_publicaciones{
    Publicacion *inicio;
    Publicacion *final;
    int size;
}Lista_publicaciones;

// Lista de usuarios
typedef struct Nodo{
    int index;
    
    char *username;
    Datos_conexion * datos_conexion;        // Estructura con los datos de la conexión actual
    Lista_publicaciones * publicaciones;    // Estructura con las publicaciones del usuario

    struct Nodo *siguiente;
    struct Nodo *anterior;
}Nodo;

typedef struct Lista{
    Nodo *inicio;
    Nodo *final;
    int size;
}Lista;

// Estructura de existencia
typedef struct Exist{
    int error;
    Nodo * nodo_usuario;
}Exist;

typedef struct Conexion{
    int error;
    Exist exist;
    Datos_conexion * datos_conexion;
}Conexion;

typedef struct Published{
    int error;
    Publicacion * publicacion;
}Published;

typedef struct{
    char username[256];  
    char username2[256];
    char filename1[256];
    char description[256];
    char filename2[256];
    int operacion;
    /*
        0 -> register
        1 -> unregister
        2 -> connect
        3 -> disconnect
        4 -> publish
        5 -> delete
        6 -> listusers
        7 -> listcontent
        8 -> getfile
    */
}Peticion;

#endif