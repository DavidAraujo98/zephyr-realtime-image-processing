CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: serialTest

serialTest: serialTest.c imageGenerator.o
	$(CC) $(CFLAGS) -o serialTest serialTest.c imageGenerator.o

imageGenerator.o: imageGenerator.c imageGenerator.h
	$(CC) $(CFLAGS) -c imageGenerator.c

clean:
	rm -f *.o *.out