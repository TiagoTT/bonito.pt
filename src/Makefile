# Simple Makefile for FastCGI.
CC=gcc
CFLAGS=-Wall -ansi -pedantic -g
LDFLAGS=-lcgi -lpq -lfcgi

PROGS=action.fcgi

install: action.fcgi
	svc -d /etc/service/bonitomail-fastcgi
	cp $< ../cgi/
	svc -u /etc/service/bonitomail-fastcgi

all: $(PROGS)

action.fcgi: action.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(PROGS)

