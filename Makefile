#paths
IDIR = include
SRC = src
LDIR = lib
ODIR = bin
BDIR = build

#compiler
CC = gcc

#compile options
CFLAGS = -Wall -g -I$(IDIR)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
TARGETS = main_HP main_HT

_DEPS = BF.h HP.h HT.h Record.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

LIBS = $(LDIR)/BF_64.a

_HPOBJS = $(SRC)/main_HP.o $(SRC)/HP.o $(SRC)/Record.o
HPOBJS = $(patsubst %,$(ODIR)/%,$(_HPOBJS))

_HTOBJS = $(SRC)/main_HT.o $(SRC)/HT.o $(SRC)/Record.o
HTOBJS = $(patsubst %,$(ODIR)/%,$(_HTOBJS))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# $(TARGETS): %: %.c HT.c HP.c Record.c
# 	$(CC) -o $@ $< HT.c HP.c Record.c -no-pie $(LIBS)

all: $(TARGETS)

main_HP: $(HPOBJS)
	$(CC) -o $(BDIR)/$@ $^ -no-pie $(LIBS)

main_HT: $(HTOBJS)
	$(CC) -o $(BDIR)/$@ $^ -no-pie $(LIBS)

clean: clean_HP clean_HT

clean_HP:
	rm -f $(ODIR)/*.o $(ODIR)/$(SRC)/*.o $(BDIR)/*
	#find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete

clean_HT:
	rm -f $(ODIR)/*.o $(ODIR)/$(SRC)/*.o $(BDIR)/*
