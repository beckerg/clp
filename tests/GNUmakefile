SUBDIRS = null standard simple params recycle breakargs

.PHONY: all ${MAKECMDGOALS} ${SUBDIRS}

all ${MAKECMDGOALS}: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@ ${MAKECMDGOALS}
