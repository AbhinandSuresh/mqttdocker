#make file of mqtt exercise at IoTuneZ
#by Abhinand Suresh

CC = gcc
#Compiler: gcc

CFLAGS = -c -Wall
#cflages variable set to compile and give warnings

LDFLAGS = -L../paho.mqtt.c/build/output/
#path to mqtt paho library

INCLUDES = -L../sqlite-amalgamation
#path for sqlite header file. suspend reading the current makefile and read sqlite header before continuing...

mqtt: mqtt.o ../paho.mqtt.c/build/output ../sqlite-amalgamation
	$(CC) final.o $(LDFLAGS) -lpaho-mqtt3as $(INCLUDES) -lsqlite3 -o mqtt

mqtt.o: final.c
	$(CC) $(CFLAGS) -c final.c

#to clean all object files created
clean:
	rm *.o mqtt
