#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "clp.h"

char version[] = "version...";
FILE *conf;
int dryrun;
int myint;
unsigned int myuint;
long mylong;
unsigned long myulong;
char *mystring = "default";

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(&verbosity),
    CLP_OPTION_DRYRUN(&dryrun),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_CONF(&conf),
    CLP_OPTION_HELP,

    CLP_OPTION(int, 'i', myint, NULL, "specify an int"),
    CLP_OPTION(uint, 'I', myuint, NULL, "specify a uint"),

    CLP_OPTION(long, 'l', mylong, NULL, "specify a long"),
    CLP_OPTION(ulong, 'L', myulong, NULL, "specify a ulong"),

    CLP_OPTION(string, 's', mystring, NULL, "specify a string"),

    // Example of a bunch of mutually exclusive boolean options.
    CLP_OPTION_BOOL('x', "yz", "specify x flag"),
    CLP_OPTION_BOOL('y', "xz", "specify y flag"),
    CLP_OPTION_BOOL('z', "xy", "specify z flag"),

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

    printf("mystring is %s %d\n", mystring, given('L'));

    if (given('x'))
        printf("x was given\n");

    if (given('y'))
        printf("y was given\n");

    if (given('z'))
        printf("z was given\n");

    return 0;
}
