#!/usr/bin/make

SHELL = /bin/sh
CC = gcc -std=gnu99
CFLAGS =
INSTALL = install -D
OBJS = error.o evaluate.o main.o options.o value.o

prefix = /usr/local
bindir = $(prefix)/bin
mandir = $(prefix)/share/man

ccalc: $(OBJS)
	$(CC) -lm -o ccalc $^

test_ccalc: test_ccalc.o
	$(CC) -lm -o $@ $^

error.o: error.c error.h
	$(CC) $(CFLAGS) -c error.c

evaluate.o: error.h evaluate.c evaluate.h options.h value.h
	$(CC) $(CFLAGS) -c evaluate.c

main.o: error.h evaluate.h main.c options.h value.h
	$(CC) $(CFLAGS) -c main.c

options.o: error.h options.c options.h
	$(CC) $(CFLAGS) -c options.c

value.o: error.h value.c value.h
	$(CC) $(CFLAGS) -c value.c

test_ccalc.o: test_ccalc.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm ccalc $(OBJS) ccalc_tester ccalc_tester.o

.PHONY: install
install:
	$(INSTALL) ccalc $(bindir)/ccalc
	$(INSTALL) ccalc.1 $(mandir)/man1/ccalc.1
	mandb
