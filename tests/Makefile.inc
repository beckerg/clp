# Copyright (c) 2011,2015,2016 Greg Becker.  All rights reserved.
#
# $Id: GNUmakefile 378 2016-01-18 14:24:15Z greg $


# The only things you probably need to change in this makefile is PROG, SRC, HDR,
# LDLIBS, and VPATH.
#
PROG	:= prog

HDR	:= main.h clp.h
SRC	:= main.c
#LDLIBS	:= -lm -lpthread -lrt
LDLIBS	:= -L../../lib -lclp

VPATH	:=

# You probably don't need to change anything below this line.

PLATFORM	:= $(shell uname -s | tr 'a-z' 'A-Z')
INCLUDE		:= -I. -I../../lib
DEBUG		:= -g -O0 -DDEBUG
CPPFLAGS	:= -D${PLATFORM}
CFLAGS		+= -Wall -g -O2 ${INCLUDE}

OBJ		:= ${SRC:.c=.o}

CSCOPE_DIRS	?= . ${VPATH} /usr/src/lib /usr/src/sys/kern /usr/src/sys/sys

INPUT := $(wildcard input/*)
OUTPUT := $(patsubst input/%, output/%, ${INPUT})
RESULT := $(patsubst output/%, result/%, ${OUTPUT})


# Always delete partially built targets.
#
.DELETE_ON_ERROR:

.PHONY:	all check clean clobber cscope debug distclean etags output tags test


all: ${PROG}

clean:
	rm -rf ${PROG} ${OBJ} *.core result

distclean clobber: clean
	rm -f cscope.files cscope*.out TAGS

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

cscope: cscope.out

cscope.out: cscope.files ${HDR} ${SRC} GNUmakefile
	cscope -bukq

cscope.files: GNUmakefile
	find ${CSCOPE_DIRS} -name \*.[chsSyl] -o -name \*.cpp > $@

debug: CFLAGS+=${DEBUG}
debug: ${PROG}

tags etags: TAGS

TAGS: cscope.files
	cat cscope.files | xargs etags -a --members --output=$@


# Use gmake's link rule to produce the target.
#
${PROG}: ${OBJ}
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@


# We make ${OBJ} depend on the makefile so that objects are rebuilt
# if the makefile changes.  Not perfect, but better to err on the
# side of caution.
#
${OBJ}: GNUmakefile


main.o: main.c main.h ../../lib/libclp.a
main.c: ../../lib/clp.h

