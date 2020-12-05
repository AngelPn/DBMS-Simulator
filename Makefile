#paths
INCLUDE = ./include
SRC = ./src
LDIR = ./lib

#compiler
CC = gcc

#compile options
CFLAGS = -Wall -g -I$(INCLUDE)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
LIBS = $(LDIR)/BF_64.a
TARGETS = main_HP main_HT
HP_OBJS = $(SRC)/HP.o $(SRC)/Record.o
HT_OBJS = $(SRC)/HT.o $(SRC)/Record.o

EXEC = ex

%.o: %.c $(INCLUDE)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

# $(TARGETS): %: %.c Record.c HT.c
# 	$(CC) -Wall -g -o $@ $<  Record.c HT.c HP.c BF_64.a -no-pie  $(CFLAGS)

main_HP: main_HP.o $(HP_OBJS)
	$(CC) -o $@ main_HP.o $(HP_OBJS) -no-pie $(LIBS)

main_HT: main_HT.o $(HT_OBJS)
	$(CC) -o $@ main_HT.o $(HT_OBJS) -no-pie $(LIBS)

valgrind:
	valgrind ${VALFLAGS} 

clean_HP:
	rm -f main_HP.o $(HP_OBJS) main_HP test
	#find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete

clean_HT:
	rm -f main_HT.o $(HT_OBJS) main_HT test
