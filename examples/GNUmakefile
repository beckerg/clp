# Copyright (c) 2021 Greg Becker.  All rights reserved.

PROGS := ex1 ex2 ex3 ex4 ex5 ex6 subcmd

SRC := clp.c ${patsubst %,%.c,${PROGS}}
OBJ := ${SRC:.c=.o}

PROG_VERSION := $(shell git describe --abbrev=8 --dirty --always --tags)

INCLUDE  := -I. -I../src
CFLAGS   += -Wall -O2 -g ${INCLUDE}
CPPFLAGS += -DPROG_VERSION=\"${PROG_VERSION}\" -DNDEBUG
VPATH    := ../src

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
	rm -f $(patsubst %.c,.%.d*,${SRC})

cleandir clobber distclean maintainer-clean: clean

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
debug: ${PROGS}

test: ${PROGS}
	for p in $^ ; do ./$$p -h > /dev/null; done

%: %.o clp.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

${OBJ}: GNUmakefile

.%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) ${INCLUDE} $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(patsubst %.c,.%.d,${SRC})
