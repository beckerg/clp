#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "clp.h"

char version[] = "version...";
char *progname;
int verbosity;
int dryrun;
FILE *cf;

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_DRYRUN(dryrun),
    CLP_OPTION_CONF(cf),
    CLP_OPTION_HELP,

    CLP_OPTION_END
};

static bool
given(int c)
{
    clp_option_t *opt = clp_option_find(optionv, c);

    return (opt && opt->given);
}

int
main(int argc, char **argv)
{
    char errbuf[CLP_ERRBUFSZ];
    int xoptind;
    int rc;
    int i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL, errbuf, &xoptind);
    if (rc) {
        fprintf(stderr, "%s: %s\n", progname, errbuf);
        exit(rc);
    }

    printf("progname is %s\n", progname);
    printf("verbosity is %d %d\n", verbosity, given('v'));
    printf("argc=%d xoptind=%d\n", argc, xoptind);

    argc -= xoptind;
    argv += xoptind;

    for (i = 0; i < argc; ++i) {
        printf("posparams: %d %s\n", i, argv[i]);
    }

    printf("dryrun is %d %d\n", dryrun, given('n'));

    if (cf) {
        printf("cf is %s %d\n", ferror(cf) ? "in error" : "open", given('C'));
    } else {
        printf("cf is nil %d\n", given('C'));
    }

    return 0;
}
