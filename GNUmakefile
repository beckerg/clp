SUBDIRS = lib test

.PHONY: all ${SUBDIRS} ${MAKECMDGOALS}

all clean clobber cscope debug tags ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
