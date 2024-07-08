CC=gcc
CFLAGS=-Wall -std=c11

tsh: src/main.c lib/tsh.c
	$(CC) -g $(CFLAGS) -o tsh src/main.c lib/tsh.c

clean:
	rm tsh
