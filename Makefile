CPP=/opt/local/bin/arm-linux-gnueabi-cpp
CC=/opt/local/bin/arm-linux-gnueabi-gcc

all:
	$(CC) -o gpio-int-test gpio-int-test.c
