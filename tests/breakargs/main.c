#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "clp.h"

char *progname;
char *delim;

struct clp_option optionv[] = {
    CLP_OPTION('d', string, delim, "", "specify a delimiter"),
    CLP_OPTION_HELP(),
    CLP_OPTION_END
};


static bool
given(int c)
{
    return !!clp_given(c, optionv, NULL);
}

int
main(int argc, char **argv)
{
    char line[1024];
    int lineno;
    int rc, i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    if (given('h'))
        exit(0);

    lineno = 0;

    while (fgets(line, sizeof(line), stdin)) {
        char **nargv;
        size_t len;
        int nargc;

        ++lineno;

        len = strlen(line);

        if (len < 1 || line[0] == '#')
            continue;

        line[--len] = '\000';

        rc = clp_breakargs(line, delim, &nargc, &nargv);
        if (rc) {
            printf("%4d: rc=%d %s\n", lineno, rc, strerror(errno));
            continue;
        }

        printf("%4d: delim=[%s] nargc=%d [%s]\n",
               lineno, delim ?: "", nargc, line);

        for (i = 0; i < nargc; ++i) {
            printf("%4d: %4d [%s]\n", lineno, i, nargv[i]);
        }

        printf("\n");
        free(nargv);
    }

    return 0;
}
