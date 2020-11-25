#paths
INCLUDE = ../BF_lib
SRC = ../BF_lib
LDIR = ../BF_lib

#compiler
CC = gcc

#compile options
CFLAGS = -I$(INCLUDE)
LIBS = $(LDIR)/BF_64.a
OBJS = $(SRC)/main.o $(SRC)/HP.o $(SRC)/Record.o

EXEC = ex

%.o: %.c $(INCLUDE)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) -no-pie $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)
	#find . -type f -not -name '*c' -not -name '*h' -not -name '*a' -not -name 'Makefile' -delete

run: $(EXEC)
	./$(EXEC)