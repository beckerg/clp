# Copyright (c) 2011,2015,2016 Greg Becker.  All rights reserved.
#
# $Id: GNUmakefile 378 2016-01-18 14:24:15Z greg $


# The only things you probably need to change in this makefile is PROG, SRC, HDR,
# LDLIBS, and VPATH.
#
LIBBASE	:= clp

AR = ar
RANLIB = ranlib

HDR	:= clp.h
SRC	:= clp.c
#LDLIBS	:= -lm -lpthread -lrt

VPATH	:=

# You probably don't need to change anything below this line.

PLATFORM	:= $(shell uname -s | tr 'a-z' 'A-Z')
INCLUDE		:= -I. -I../lib -I../../src/include
DEBUG		:= -O0 -DDEBUG -fno-omit-frame-pointer
CPPFLAGS	:= -D${PLATFORM}
CFLAGS		+= -Wall -O2 -g3 ${INCLUDE}

OBJ		:= ${SRC:.c=.o}

#CSCOPE_DIRS	?= . ${VPATH} /usr/src/lib /usr/src/sys/kern /usr/src/sys/sys
CSCOPE_DIRS	?= . ${VPATH} /usr/src/lib /usr/src/sys


# Always delete partially built targets.
#
.DELETE_ON_ERROR:

.PHONY:	all asan check clean clobber cscope debug distclean etags tags


all: ${LIBBASE}.h lib${LIBBASE}.a

asan: CFLAGS+=${DEBUG}
asan: CFLAGS+=-fsanitize=address -fsanitize=undefined
asan: LDLIBS+=-fsanitize=address -fsanitize=undefined
asan: ${LIBBASE}.h lib${LIBBASE}.a

clean:
	rm -f lib${LIBBASE}.a ${OBJ} *.core

clobber distclean: clean
	rm -f cscope.files cscope*.out TAGS

check test:

cscope: cscope.out

cscope.out: cscope.files ${HDR} ${SRC} GNUmakefile
	cscope -bukq

cscope.files: GNUmakefile
	find ${CSCOPE_DIRS} -name \*.[chsSyl] -o -name \*.cpp > $@

debug: CFLAGS+=${DEBUG}
debug: ${LIBBASE}.h lib${LIBBASE}.a

output:

tags etags: TAGS

TAGS: cscope.files
	cat cscope.files | xargs etags -a --members --output=$@


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
