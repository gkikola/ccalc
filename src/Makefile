#!/usr/bin/make

SHELL = /bin/sh
CC = gcc -std=gnu99
CFLAGS =
INSTALL = install
OBJS = error.o evaluate.o main.o options.o value.o

bindir = /usr/local/bin

ccalc: $(OBJS)
	$(CC) -lm -o ccalc $^

ccalc_tester: ccalc_tester.o
	$(CC) -lm -o ccalc_tester $^

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

ccalc_tester.o: ccalc_tester.c
	$(CC) $(CFLAGS) -c ccalc_tester.c

.PHONY: clean
clean:
	rm ccalc $(OBJS) ccalc_tester ccalc_tester.o

.PHONY: install
install:
	$(INSTALL) ccalc $(bindir)/ccalc

