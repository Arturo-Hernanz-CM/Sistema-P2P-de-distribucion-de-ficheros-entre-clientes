# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -fPIC -pthread -DRPC_SVC_FG

# Librerías para el linker
LDLIBS = -lpthread -ldl

CPPFLAGS += -D_REENTRANT

# Detección del sistema
UNAME_S := $(shell uname -s)

# Flags de compilación

ifeq ($(UNAME_S),Linux)
	CFLAGS += -I/usr/include/tirpc 
	CFLAGS += -lrt
	LDLIBS += -ltirpc
endif


ifeq ($(UNAME_S),Darwin)
	CFLAGS += -arch arm64
	LDLIBS += -arch arm64
endif


# Archivos fuente
SRCS_SERVER = servidor.c claves.c lines.c tuplas_clnt.c tuplas_xdr.c
SRCS_SERVER_LOG = log_rpc.c tuplas_svc.c tuplas_xdr.c
SRCS_PROXY = proxy.c lines.c

# Archivos objeto
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_SERVER_LOG = $(SRCS_SERVER_LOG:.c=.o)
OBJS_PROXY = $(SRCS_PROXY:.c=.o)

# Librería compartida
LIB_NAME = libclaves.so

# Ejecutables
TARGET_SERVER = servidor
TARGET_SERVER_LOG = servidor_log

# Regla por defecto: compilar todo
all: $(TARGET_SERVER) $(TARGET_SERVER_LOG) $(LIB_NAME)

# Compilar el servidor
$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJS_SERVER) $(LDLIBS)

# Compilar el servidor con logs
$(TARGET_SERVER_LOG): $(OBJS_SERVER_LOG)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER_LOG) $(OBJS_SERVER_LOG) $(LDLIBS)

# Crear la librería compartida
$(LIB_NAME): $(OBJS_PROXY)
	$(CC) -shared -o $(LIB_NAME) $(OBJS_PROXY)

# Regla para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS_SERVER) $(OBJS_PROXY) $(OBJS_SERVER_LOG) $(TARGET_SERVER) $(TARGET_SERVER_LOG) $(LIB_NAME)