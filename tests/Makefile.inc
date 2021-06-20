# Copyright (c) 2011,2015,2016,2021 Greg Becker.  All rights reserved.

PROG := prog

HDR := clp.h
SRC := main.c ../strerror.c
OBJ := ${SRC:.c=.o}

INCLUDE  := -I. -I../../lib
CFLAGS   += -Wall -O2 -g3 ${INCLUDE}
CPPFLAGS += -DNDEBUG
LDLIBS   += -L../../lib -lclp -Wl,--wrap=strerror

INPUT := $(wildcard input/*)
OUTPUT := $(patsubst input/%, output/%, ${INPUT})
RESULT := $(patsubst output/%, result/%, ${OUTPUT})


.DELETE_ON_ERROR:

.PHONY:	all asan check clean clobber debug distclean maintainer-clean output test


all: ${PROG}

asan: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
asan: CFLAGS += -O0 -fno-omit-frame-pointer
asan: CFLAGS += -fsanitize=address -fsanitize=undefined
asan: LDLIBS += -fsanitize=address -fsanitize=undefined
asan: ${PROG}

clean:
	rm -rf ${PROG} ${OBJ} *.core result

clobber distclean: clean

check test: ${OUTPUT} ${RESULT}

output/%: input/%
	env -i PATH=${PATH} bash -x input/${@F} > $@ 2>&1 || true

result/%: input/% ${PROG}
	mkdir -p ${@D}
	env -i PATH=${PATH} bash -x input/${@F} > $@ 2>&1 || true
	@echo "checking $(abspath result/${@F})"
	diff output/${@F} result/${@F}

output:
	touch ${INPUT}
	${MAKE} ${OUTPUT}

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
debug: ${PROG}

# Use gmake's link rule to produce the target.
#
${PROG}: ${OBJ}
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@


# We make ${OBJ} depend on the makefile so that objects are rebuilt
# if the makefile changes.  Not perfect, but better to err on the
# side of caution.
#
${OBJ}: GNUmakefile


main.o: main.c ../../lib/libclp.a
main.c: ../../lib/clp.h

