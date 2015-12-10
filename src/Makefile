#!/usr/bin/make

SHELL = /bin/sh
CC = gcc
CFLAGS = 
OBJS = evaluate.o main.o options.o value.o

ccalc: $(OBJS)
	$(CC) -lm -o ccalc $^

ccalc_tester: ccalc_tester.o
	$(CC) -lm -o ccalc_tester $^

evaluate.o: define.h evaluate.c evaluate.h options.h value.h
	$(CC) $(CFLAGS) -c evaluate.c

main.o: define.h evaluate.h main.c options.h value.h
	$(CC) $(CFLAGS) -c main.c

options.o: define.h options.c options.h
	$(CC) $(CFLAGS) -c options.c

value.o: define.h value.c value.h
	$(CC) $(CFLAGS) -c value.c

ccalc_tester.o: ccalc_tester.c
	$(CC) $(CFLAGS) -c ccalc_tester.c

.PHONY: clean
clean:
	rm ccalc $(OBJS) ccalc_tester.o
