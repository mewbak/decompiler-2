CC      := gcc
CFLAGS  := -I./include
COUT    := execer
SOURCES := $(wildcard *.c)

.PHONY: all

all: core

core:
	$(CC) $(CFLAGS) -o $(COUT) $(SOURCES)
