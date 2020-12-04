#paths
INCLUDE = ../BF_lib
SRC = ../BF_lib
LDIR = ../BF_lib

#compiler
CC = gcc

#compile options
CFLAGS = -Wall -g -I$(INCLUDE)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
LIBS = $(LDIR)/BF_64.a
TARGETS = main mainHT
OTHER_OBJS = $(SRC)/HP.o $(SRC)/Record.o $(SRC)/HT.o

EXEC = ex

%.o: %.c $(INCLUDE)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

# $(TARGETS): %: %.c Record.c HT.c
# 	$(CC) -Wall -g -o $@ $<  Record.c HT.c HP.c BF_64.a -no-pie  $(CFLAGS)

main: main.o $(OTHER_OBJS)
	$(CC) -o $@ main.o $(OTHER_OBJS) -no-pie $(LIBS)

mainHT: mainHT.o $(OTHER_OBJS)
	$(CC) -o $@ mainHT.o $(OTHER_OBJS) -no-pie $(LIBS)

valgrind:
	valgrind ${VALFLAGS} 

clean:
	find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete
