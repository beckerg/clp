# Copyright (c) 2011,2015,2016,2021 Greg Becker.  All rights reserved.

PROG := prog

HDR := clp.h
SRC := clp.c main.c ../strerror.c
OBJ := ${SRC:.c=.o}

INCLUDE  := -I. -I../../src
CFLAGS   += -Wall -Wextra -O2 -g3 ${INCLUDE}
CPPFLAGS += -DNDEBUG
LDLIBS   += -Wl,--wrap=strerror
VPATH    := ../../src

INPUT := $(wildcard input/*)
OUTPUT := $(patsubst input/%, output/%, ${INPUT})
RESULT := $(patsubst output/%, result/%, ${OUTPUT})


.DELETE_ON_ERROR:

.PHONY:	all asan check clean clobber debug distclean maintainer-clean output test

.PRECIOUS: result/%

all: ${PROG}

asan: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
asan: CFLAGS += -O0 -fno-omit-frame-pointer
asan: CFLAGS += -fsanitize=address -fsanitize=undefined
asan: LDLIBS += -fsanitize=address -fsanitize=undefined
asan: ${PROG}

clean:
	rm -rf ${PROG} ${OBJ} *.core result
	rm -f $(patsubst %.c,.%.d*,${SRC})

clobber distclean maintainer-clean: clean

check test: ${OUTPUT} ${RESULT}

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
debug: ${PROG}

output/%: input/%
	env -i PATH=${PATH} bash -x input/${@F} > $@ 2>&1 || true

output:
	touch ${INPUT}
	${MAKE} ${OUTPUT}

result/%: input/% ${PROG}
	mkdir -p ${@D}
	env -i PATH=${PATH} bash -x input/${@F} > $@ 2>&1 || true
	@echo "checking $(abspath result/${@F})"
	diff -I '^\+\+' output/${@F} result/${@F}

${PROG}: ${OBJ}
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

${OBJ}: GNUmakefile ../Makefile.inc

.%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) ${INCLUDE} $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(patsubst %.c,.%.d,${SRC})
