# @name Makefile
# @author Robert Sowula <e1170475@student.tuwien.ac.at>

CC = gcc
DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
LDFLAGS = -lm

OBJECTS = forkfft.o readWriteFD.o

.PHONY: all clean
all: forkfft

forkfft: $(OBJECTS)
	$(CC) -o $@ $^  $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

forkfft.o: forkfft.c forkfft.h
readWriteFD.o: readWriteFD.c forkfft.h

clean:
	rm -rf *.o forkfft
