SUBDIRS = null standard simple params recycle breakargs

.PHONY: all ${SUBDIRS} ${MAKECMDGOALS}

all check clean clobber cscope debug output tags test ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
