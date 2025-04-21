#ifndef proxymq_h
#define proxymq_h

#include "estructuras.h"
#include <stdint.h>

int destroy(void);
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3);
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3);
int delete_key(int key);
int exist(int key);
int tratar_peticion(Peticion * peticion);
int enviar_peticion(int socket_cliente, Peticion * peticion);
double deserializar_double(uint64_t v_value2);
uint64_t serializar_double(double value);

#endif