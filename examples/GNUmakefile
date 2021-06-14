# Copyright (c) 2021 Greg Becker.  All rights reserved.

PROGS := ex1 ex2 ex3 ex4 ex5

SRC := ${patsubst %,%.c,${PROGS}} clp.c
OBJ := ${SRC:.c=.o}

#LDLIBS := -pthread

PROG_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)
PLATFORM     := $(shell uname -s | tr 'a-z' 'A-Z')

INCLUDE   := -I. -I../lib
CDEFS     := -DPROG_VERSION=\"${PROG_VERSION}\"
CDEFS     += -DNDEBUG
CPPFLAGS  := ${CDEFS}

CFLAGS    += -std=c11 -Wall -O2 -g ${INCLUDE}
CFLAGS    += -march=native
DEBUG     := -O0 -DCLP_DEBUG -UNDEBUG -fno-omit-frame-pointer

VPATH := ../lib

# Always delete partially built targets.
#
.DELETE_ON_ERROR:
.NOTPARALLEL:

.PHONY:	all clean clobber debug distclean maintainer-clean test


all: ${PROGS}

clean:
	rm -f ${PROGS} ${OBJ} *.core
	rm -f $(patsubst %.c,.%.d,${SRC})

cleandir clobber distclean maintainer-clean: clean

debug: CFLAGS += ${DEBUG}
debug: ${PROGS}

test: ${PROGS}
	for p in $^ ; do ./$$p -h; done

# Use gmake's link rule to produce the target.
#
%: %.o clp.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@


# We make ${OBJ} depend on the makefile so that all objects are rebuilt
# if the makefile changes.
#
${OBJ}: GNUmakefile

# Automatically generate/maintain dependency files.
#
.%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) ${INCLUDE} $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(patsubst %.c,.%.d,${SRC})
