SUBDIRS = tests examples

.PHONY: all ${MAKECMDGOALS} ${SUBDIRS}

all ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
