#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "clp.h"

char *progname;

int
main(int argc, char **argv)
{
    int rc, i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, NULL, NULL);
    if (rc)
        return rc;

    printf("progname is %s\n", progname);
    printf("argc=%d optind=%d\n", argc, optind);

    argc -= optind;
    argv += optind;

    for (i = 0; i < argc; ++i) {
        printf("posparams: %d %s\n", i, argv[i]);
    }

    return 0;
}
