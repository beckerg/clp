SUBDIRS = lib tests

.PHONY: all check ${SUBDIRS} ${MAKECMDGOALS}

all check clean clobber cscope debug output tags ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
