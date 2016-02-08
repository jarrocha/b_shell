#Makefile for myshell

GCC = gcc
CFLAGS = -Wall -g -std=gnu99

all:
	$(GCC) $(CFLAGS) -o bscshell.o mainv0.3.c
clean:
	rm -rf bscshell.o
