CC = gcc
CFLAGS = -c -Wall
LIBS = -lm

all: test

test: main.o datagram.o fragment.o output.o reassembled.o
	$(CC) main.o datagram.o fragment.o output.o reassembled.o -o test $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) main.c $(LIBS)

datagram.o: datagram.c
	$(CC) $(CFLAGS) datagram.c 

fragment.o: fragment.c
	$(CC) $(CFLAGS) fragment.c 

output.o: output.c
	$(CC) $(CFLAGS) output.c $(LIBS)

reassembled.o: reassembled.c
	$(CC) $(CFLAGS) reassembled.c

clean:
	rm -rf *.o test


