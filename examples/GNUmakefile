# Copyright (c) 2021 Greg Becker.  All rights reserved.

PROGS := ex1 ex2 ex3 ex4 ex5 ex6 subcmd

SRC := ${patsubst %,%.c,${PROGS}} clp.c
OBJ := ${SRC:.c=.o}

PROG_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)

INCLUDE  := -I. -I../lib
CFLAGS   += -std=c11 -Wall -O2 -g ${INCLUDE}
CFLAGS   += -march=native
CPPFLAGS += -DPROG_VERSION=\"${PROG_VERSION}\" -DNDEBUG
VPATH    := ../lib

# Always delete partially built targets.
#
.DELETE_ON_ERROR:
.NOTPARALLEL:

.PHONY:	all asan check clean clobber debug distclean maintainer-clean test


all: ${PROGS}

asan: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
asan: CFLAGS += -O0 -fno-omit-frame-pointer
asan: CFLAGS += -fsanitize=address -fsanitize=undefined
asan: LDLIBS += -fsanitize=address -fsanitize=undefined
asan: ${PROGS}

clean:
	rm -f ${PROGS} ${OBJ} *.core
	rm -f $(patsubst %.c,.%.d,${SRC})

cleandir clobber distclean maintainer-clean: clean

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
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
