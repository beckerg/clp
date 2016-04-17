#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "clp.h"

#define NDEBUG

char version[] = "version...";
FILE *conf;
int dryrun;
int myint;
unsigned int myuint;
long mylong;
unsigned long myulong;

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(&verbosity),
    CLP_OPTION_DRYRUN(&dryrun),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_CONF(&conf),
    CLP_OPTION_HELP,

    // Example of a simple integer argument option
    CLP_OPTION_INT('i', myint, NULL, "specify an int"),

    // Example of a simple unsigned integer argument option
    CLP_OPTION_UINT('I', myuint, NULL, "specify a uint"),

    // Example of a simple long argument option
    CLP_OPTION_LONG('l', mylong, NULL, "specify a long"),

    // Example of a simple unsigned long argument option
    CLP_OPTION_LONG('L', myulong, NULL, "specify a ulong"),

    // Example of a bunch of mutually exclusive boolean options.
    CLP_OPTION_BOOL('x', "yz", "specify x flag"),
    CLP_OPTION_BOOL('y', "xz", "specify y flag"),
    CLP_OPTION_BOOL('z', "xy", "specify z flag"),

    { .optopt = 0 }
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
    int optind;
    int rc;
    int i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL, errbuf, &optind);
    if (rc) {
        fprintf(stderr, "%s: %s\n", progname, errbuf);
        exit(rc);
    }

    printf("progname is %s\n", progname);
    printf("verbosity is %d %d\n", verbosity, given('v'));
    printf("argc=%d optind=%d\n", argc, optind);

    argc -= optind;
    argv += optind;

    for (i = 0; i < argc; ++i) {
        printf("posparams: %d %s\n", i, argv[i]);
    }

    printf("dryrun is %d %d\n", dryrun, given('n'));
    printf("conf is %p %d\n", conf, given('C'));

    printf("myint is %d %d\n", myint, given('i'));
    printf("myuint is %u %d\n", myuint, given('I'));

    printf("mylong is %ld %d\n", mylong, given('l'));
    printf("myulong is %lu %d\n", myulong, given('L'));

    if (given('x'))
        printf("x was given\n");

    if (given('y'))
        printf("y was given\n");

    if (given('z'))
        printf("z was given\n");

    return 0;
}
