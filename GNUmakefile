SUBDIRS = lib check

.PHONY: all ${SUBDIRS} ${MAKECMDGOALS}

all check clean clobber cscope debug tags ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
