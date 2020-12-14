#paths
IDIR = ../include
LDIR = ../lib
ODIR = ../bin
BDIR = ../build

#compiler
CC = gcc

#compile options
CFLAGS = -Wall -g -I$(IDIR)
VALFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes
TARGETS = main_HP main_HT

_DEPS = BF.h HP.h HT.h Record.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

LIBS = $(LDIR)/BF_64.a

_HPOBJS = main_HP.o HP.o Record.o
HPOBJS = $(patsubst %,$(ODIR)/%,$(_HPOBJS))

_HTOBJS = main_HT.o HT.o Record.o
HTOBJS = $(patsubst %,$(ODIR)/%,$(_HTOBJS))

#dir_guard=$(shell [ ! -d $(@D) ] && mkdir -p $(@D)) #if dir exists dont make a new one

$(ODIR)/%.o: %.c $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

# $(TARGETS): %: %.c HT.c HP.c Record.c
# 	$(CC) -o $@ $< HT.c HP.c Record.c -no-pie $(LIBS)

all: $(TARGETS)

main_HP: $(HPOBJS)
	@mkdir -p $(BDIR)
	$(CC) -o $(BDIR)/$@ $^ -no-pie $(LIBS)

main_HT: $(HTOBJS)
	@mkdir -p $(BDIR)
	$(CC) -o $(BDIR)/$@ $^ -no-pie $(LIBS)

clean: 
	rm -f $(ODIR)/*.o $(BDIR)/*
	rmdir $(ODIR) $(BDIR)
