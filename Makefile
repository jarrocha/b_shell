#Makefile for myshell

GCC = gcc
CFLAGS = -Wall -g -std=c99

all:
	$(GCC) $(CFLAGS) -o bas_shell.o mainv0.3.c builtin.c
clean:
	rm -rf *.o
