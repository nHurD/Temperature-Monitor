CC = /opt/local/bin/arm-linux-gnueabi-gcc
CFLAGS = -fPIC -shared -c 
FLAGS = -g -I ./include/
LD = /opt/local/bin/arm-linux-gnueabi-ld

SOURCES = src/w1.c 
OBJECTS = $(SOURCES: .c=.o) 
TARGET = libw1.so
PROG_NAME = get_temperature

HOST = 192.168.1.7
USER = root

all: main

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main: $(TARGET)
	$(CC) $(FLAGS) -L. -L./lib -lw1 -lsqlite3 -o $(PROG_NAME) src/main.c src/sqlite.c

clean:
	rm $(PROG_NAME) $(TARGET)

deploy: main
	scp $(TARGET) $(USER)@$(HOST):
	scp $(PROG_NAME) $(USER)@$(HOST):

	
