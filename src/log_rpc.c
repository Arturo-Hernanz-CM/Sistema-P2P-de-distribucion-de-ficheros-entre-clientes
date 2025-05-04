#include "tuplas.h"
#include <stdio.h> 
#include <stdlib.h>
#include <rpc/pmap_clnt.h>

int * log_1_svc(arg_log argp, struct svc_req *rqstp)
{
    static int result = 0;
    char string[291];
    sprintf(string, "%s %s %s", argp.nombre, argp.operacion, argp.fecha_hora);
    printf("%s\n", string);
    return &result;
}

int * log_filename_1_svc(arg_log_filename argp, struct svc_req *rqstp)
{
    static int result = 0;
    char string[547];
    sprintf(string, "%s %s %s %s", argp.nombre, argp.operacion, argp.filename, argp.fecha_hora);
    printf("%s\n", string);
    return &result;
}