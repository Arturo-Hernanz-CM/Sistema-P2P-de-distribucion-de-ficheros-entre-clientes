# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -fPIC -pthread -lrt

# Archivos fuente
SRCS_SERVER = servidor.c claves.c lines.c
SRCS_PROXY = proxy.c lines.c

# Archivos objeto
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_PROXY = $(SRCS_PROXY:.c=.o)

# Librería compartida
LIB_NAME = libclaves.so

# Ejecutables
TARGET_SERVER = servidor
TARGET_CLIENT0 = app-cliente-0
TARGET_CLIENT1 = app-cliente-1
TARGET_CLIENT6 = app-cliente-6
TARGET_CLIENT2 = app-cliente-2
TARGET_CLIENT3 = app-cliente-3
TARGET_CLIENT4 = app-cliente-4
TARGET_CLIENT5 = app-cliente-5

# Regla por defecto: compilar todo
all: $(TARGET_SERVER) $(LIB_NAME)

# Compilar el servidor
$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJS_SERVER)

# Crear la librería compartida
$(LIB_NAME): $(OBJS_PROXY)
	$(CC) -shared -o $(LIB_NAME) $(OBJS_PROXY)

# Regla para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS_SERVER) $(OBJS_PROXY) $(TARGET_SERVER) $(LIB_NAME)