SUBDIRS = null standard simple params

.PHONY: all ${SUBDIRS} ${MAKECMDGOALS}

all check clean clobber cscope debug output tags ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
