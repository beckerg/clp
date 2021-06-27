# Copyright (c) 2011,2015,2016,2021 Greg Becker.  All rights reserved.

LIBBASE	:= clp

HDR := clp.h
SRC := clp.c
OBJ := ${SRC:.c=.o}

INCLUDE  := -I.
CFLAGS   += -Wall -Wextra -O2 -g3 ${INCLUDE}
CPPFLAGS += -DNDEBUG

RANLIB ?= ranlib
AR ?= ar


.DELETE_ON_ERROR:

.PHONY:	all asan clean clobber debug distclean maintainer-clean


all: lib${LIBBASE}.a

asan: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
asan: CFLAGS += -O0 -fno-omit-frame-pointer
asan: CFLAGS += -fsanitize=address -fsanitize=undefined
asan: LDLIBS += -fsanitize=address -fsanitize=undefined
asan: lib${LIBBASE}.a

clean:
	rm -f lib${LIBBASE}.a ${OBJ} *.core
	rm -f $(patsubst %.c,.%.d*,${SRC})

clobber distclean maintainer-clean: clean

debug: CPPFLAGS += -UNDEBUG -DCLP_DEBUG
debug: CFLAGS += -O0 -fno-omit-frame-pointer
debug: lib${LIBBASE}.a

lib${LIBBASE}.a: ${OBJ}
	${AR} cr lib${FPA}${LIBBASE}.a ${OBJ}
	${RANLIB} lib${FPA}${LIBBASE}.a

${OBJ}: GNUmakefile

.%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) ${INCLUDE} $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(patsubst %.c,.%.d,${SRC})
