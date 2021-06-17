# Copyright (c) 2011,2015,2016,20201 Greg Becker.  All rights reserved.

LIBBASE	:= clp

HDR := clp.h
SRC := clp.c
OBJ := ${SRC:.c=.o}

INCLUDE  := -I. -I../lib -I../../src/include
CFLAGS   += -Wall -O2 -g3 ${INCLUDE}
CPPFLAGS += -DNDEBUG

AR = ar
RANLIB = ranlib


.DELETE_ON_ERROR:

.PHONY:	all asan check clean clobber debug distclean maintainer-clean test


all: ${LIBBASE}.h lib${LIBBASE}.a

asan: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
asan: CFLAGS += -O0 -fno-omit-frame-pointer
asan: CFLAGS += -fsanitize=address -fsanitize=undefined
asan: LDLIBS += -fsanitize=address -fsanitize=undefined
asan: ${LIBBASE}.h lib${LIBBASE}.a

clean:
	rm -f lib${LIBBASE}.a ${OBJ} *.core

clobber distclean: clean

check test:

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
debug: ${LIBBASE}.h lib${LIBBASE}.a

output:

# Use gmake's link rule to produce the target.
#
lib${LIBBASE}.a: ${OBJ}
	${AR} cr lib${FPA}${LIBBASE}.a ${OBJ}
	${RANLIB} lib${FPA}${LIBBASE}.a



# We make ${OBJ} depend on the makefile so that objects are rebuilt
# if the makefile changes.  Not perfect, but better to err on the
# side of caution.
#
${OBJ}: GNUmakefile


clp.o: clp.c clp.h
