# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -g -fPIC -pthread -lrt

# Archivos fuente
SRCS_SERVER = servidor-sock.c claves.c lines.c
SRCS_PROXY = proxy-sock.c lines.c
SRCS_CLIENT0 = clientes/app-cliente-0.c
SRCS_CLIENT1 = clientes/app-cliente-1.c
SRCS_CLIENT6 = clientes/app-cliente-6.c
SRCS_CLIENT2 = clientes/app-cliente-2.c
SRCS_CLIENT3 = clientes/app-cliente-3.c
SRCS_CLIENT4 = clientes/app-cliente-4.c
SRCS_CLIENT5 = clientes/app-cliente-5.c

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
all: $(TARGET_SERVER) $(LIB_NAME) $(TARGET_CLIENT0) $(TARGET_CLIENT1) $(TARGET_CLIENT6) $(TARGET_CLIENT2) $(TARGET_CLIENT3) $(TARGET_CLIENT4) $(TARGET_CLIENT5)

# Compilar el servidor
$(TARGET_SERVER): $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJS_SERVER)

# Crear la librería compartida
$(LIB_NAME): $(OBJS_PROXY)
	$(CC) -shared -o $(LIB_NAME) $(OBJS_PROXY)

# Compilar los cliente-s y vincular la librería
$(TARGET_CLIENT0): $(SRCS_CLIENT0)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT0) $(SRCS_CLIENT0) -L. -lclaves

$(TARGET_CLIENT1): $(SRCS_CLIENT1)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT1) $(SRCS_CLIENT1) -L. -lclaves

$(TARGET_CLIENT6): $(SRCS_CLIENT6)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT6) $(SRCS_CLIENT6) -L. -lclaves

$(TARGET_CLIENT2): $(SRCS_CLIENT2)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT2) $(SRCS_CLIENT2) -L. -lclaves

$(TARGET_CLIENT3): $(SRCS_CLIENT3)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT3) $(SRCS_CLIENT3) -L. -lclaves

$(TARGET_CLIENT4): $(SRCS_CLIENT4)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT4) $(SRCS_CLIENT4) -L. -lclaves

$(TARGET_CLIENT5): $(SRCS_CLIENT5)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT5) $(SRCS_CLIENT5) -L. -lclaves

# Regla para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJS_SERVER) $(OBJS_PROXY) $(TARGET_SERVER) $(TARGET_CLIENT0) $(TARGET_CLIENT1) $(TARGET_CLIENT6) $(TARGET_CLIENT2) $(TARGET_CLIENT3) $(TARGET_CLIENT4) $(TARGET_CLIENT5) $(LIB_NAME)