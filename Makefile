CC=gcc
CFLAGS=-Wall -g

all: shell

shell: shell.o readcmd.o

clean:
	rm -f shell shell.o readcmd.o
