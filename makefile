# Makefile for dynamicservices* 

CC = g++
CFLAGS = -O3 -O2 -std=c++11 -ljsoncpp -lcurl -lredox -lev -lhiredis
#CFLAGS = -g -std=c++11 -ljsoncpp -lcurl -lredox -lev -lhiredis
SRCDIR = src
OBJDIR = obj
#INCDIR = include
#VPATH = $(SRCDIR) $(INCDIR)

_OBJ = Order.o CaMicroscopeService.o main.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

all: dynamicservices
	
dynamicservices: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o dynamicservices
	
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

clean : 
	rm $(OBJ)
