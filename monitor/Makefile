CC = /opt/local/bin/arm-linux-gnueabi-gcc
CFLAGS = -fPIC -shared -c 
FLAGS = -g -D D_PTHREAD -pthread -I ./include/ -I ./include/glib-2.0
LD = /opt/local/bin/arm-linux-gnueabi-ld

SOURCES = src/w1.c 
OBJECTS = $(SOURCES: .c=.o) 
TARGET = libw1.so
PROG_NAME = get_temperature
PROG_SRC = src/main.c src/sqlite.c src/net.c src/net_pthread.c src/callbacks.c src/daemon.c src/daemon_options.c src/sqlite_thread.c 

HOST = 192.168.1.3
USER = jrsteele

all: main

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main: $(TARGET)
	$(CC) $(FLAGS) -L. -L./lib -lw1 -lsqlite3 -lglib-2.0 -o $(PROG_NAME) $(PROG_SRC)

clean:
	rm $(PROG_NAME) $(TARGET)

deploy: main
	scp $(TARGET) $(USER)@$(HOST):
	scp $(PROG_NAME) $(USER)@$(HOST):

	
