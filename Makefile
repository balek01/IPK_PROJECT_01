CC=gcc
CFLAGS=-Wall -g
OUTPUT=ipkcpc

build: ipkcpc.c ipkcpc.h
	$(CC) $(CFLAGS) ipkcpc.c -o $(OUTPUT)

run: ipkcpc
	./$(OUTPUT)

clean:
	rm -f $(OUTPUT)