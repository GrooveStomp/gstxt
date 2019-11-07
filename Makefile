#******************************************************************************
# File: Makefile
# Created: 2019-10-16
# Updated: 2019-11-06
# Author: Aaron Oman
# Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
#******************************************************************************
CC       = /usr/bin/gcc
INC     += $(shell sdl2-config --cflags)
HEADERS  = $(wildcard *.h)
LIBS    += $(shell sdl2-config --libs) -lSDL2main -lm
CFLAGS  += -std=c11 -pedantic -Wall -D_GNU_SOURCE

SRC_DEP  =
SRC      = $(wildcard *.c)
OBJFILES = $(patsubst %.c,%.o,$(SRC))
LINTFILES= $(patsubst %.c,__%.c,$(SRC)) $(patsubst %.c,_%.c,$(SRC))

DBGDIR = debug
DBGOBJ = $(addprefix $(DBGDIR)/,$(OBJFILES))
DBGEXE = $(DBGDIR)/demo
DBGFLG = -g -Og

DEFAULT_GOAL := $(debug)
.PHONY: clean debug

debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJ)
	$(CC) -o $@ $^ $(LIBS)

$(DBGDIR)/%.o: %.c $(HEADERS) $(SRC_DEP)
	@mkdir -p $(@D)
	$(CC) -c $*.c $(INC) $(CFLAGS) $(DBGFLG) -o $@

clean:
	rm -rf core debug ${LINTFILES} ${DBGOBJ} ${RELOBJ}
