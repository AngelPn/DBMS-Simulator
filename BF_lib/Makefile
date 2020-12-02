# #paths
# INCLUDE = ../BF_lib
# SRC = ../BF_lib
# LDIR = ../BF_lib

# #compiler
# CC = gcc

# #compile options
# CFLAGS = -I$(INCLUDE)
# LIBS = $(LDIR)/BF_64.a
# OBJS = $(SRC)/HP.o $(SRC)/Record.o  $(SRC)/mainHT.o  $(SRC)/HT.o

# EXEC = ex

# %.o: %.c $(INCLUDE)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# $(EXEC): $(OBJS)
# 	$(CC) -o $@ $(OBJS) -no-pie $(LIBS)

# clean:
# 	find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete
# 	#rm -f $(OBJS) $(EXEC)
	
# run: $(EXEC)
# 	./$(EXEC)
CC=gcc
CFLAGS=
DEPS = HT.h Record.h HP.c
TARGETS = main mainHT
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes \

.PHONY: clean all

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(TARGETS)

$(TARGETS): %: %.c Record.c HT.c
	$(CC) -Wall -g -o $@ $<  Record.c HT.c HP.c BF_64.a -no-pie  $(CFLAGS)

valgrind:
	valgrind ${VALFLAGS} 

clean:
	find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete
	rm -f $(TARGETS)