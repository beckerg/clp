
all asan check clean clobber debug native test:
	echo running \"gmake ${MAKEFLAGS} $@\"
	gmake ${MAKEFLAGS} $@
