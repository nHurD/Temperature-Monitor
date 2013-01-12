CPP=/opt/local/bin/arm-linux-gnueabi-cpp
CC=/opt/local/bin/arm-linux-gnueabi-gcc

CFLAGS=-g -o w1

all:
	$(CC) $(CFLAGS) w1.*

