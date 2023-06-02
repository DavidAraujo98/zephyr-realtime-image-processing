CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: serialComm

serialComm: serialComm.c imageGenerator.o
	$(CC) $(CFLAGS) -o serialComm serialComm.c imageGenerator.o
	
imageGenerator.o: imageGenerator.c imageGenerator.h
	$(CC) $(CFLAGS) -c imageGenerator.c

clean:
	rm -f serialComm *.o *.out