#include "tuplas.h"
#include <stdio.h> 
#include <stdlib.h>
#include <rpc/pmap_clnt.h>

bool_t log_1_svc(arg_log argp,int *result, struct svc_req *rqstp)
{
    static int resultado = 0;
    char string[291];
    resultado = sprintf(string, "%s %s %s", argp.nombre, argp.operacion, argp.fecha_hora);
    *result = resultado;
    if(resultado <= 0){
        return FALSE;
    };
    printf("%s\n", string);
    return TRUE;
}

bool_t log_filename_1_svc(arg_log_filename argp, int* result, struct svc_req *rqstp)
{
    static int resultado = 0;
    char string[547];
    resultado = sprintf(string, "%s %s %s %s", argp.nombre, argp.operacion, argp.filename, argp.fecha_hora);
    *result = resultado;
    if(resultado <= 0){
        return FALSE;
    }
    printf("%s\n", string);
    return FALSE;
}