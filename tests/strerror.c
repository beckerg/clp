/*
 * Copyright (c) 2016 Greg Becker.  All rights reserved.
 */
#if __linux__
#define _DEFAULT_SOURCE
#endif

#include <errno.h>
#include <string.h>

/* Override strerror() for test programs so that we can normalize
 * error messages between platforms (e.g., FreeBSD and Linux).
 */
char *
__wrap_strerror(int errnum)
{
    static char errbuf[128];

    switch (errnum) {
    case ERANGE:
        strcpy(errbuf, "ERANGE");
        break;

    default:
        strerror_r(errnum, errbuf, sizeof(errbuf));
        break;
    }

    return errbuf;
}
