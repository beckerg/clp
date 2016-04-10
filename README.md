# clp
Simple command line processor

clp is (yet another) command line processor that aims to bring consistency and simplicity
to command line parsing.  It is a one-pass parser/processor, in the sense that you can
simply call clp_parsev() to parse and process a given argument vector.  There is no need
to run external programs nor create/initialize objects.

By default clp builds as a static library, but your best best is probably to simply include
clp/lib/clp.[ch] into your project.

For example, here's a very simple program that accepts one of three mutually exclusive
options and one or more positional parameters:

`#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "clp.h"

char *progname;
int verbosity;

int myint;

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(&verbosity),
    CLP_OPTION_HELP,

    { // Example of a simple integer argument option
        .optopt = 'i', .argname = "int",
        .help = "specify an int",
        .convert = clp_convert_int, .result = &myint, .cvtarg = (void *)10,
    },

    { .optopt = 'x', .excludes = "y", .help = "specify the x flag", },
    { .optopt = 'y', .excludes = "xz", .help = "specify the y flag", },
    { .optopt = 'z', .excludes = "y", .help = "specify the z flag", },

    { .optopt = 0 }
};

clp_posparam_t posparamv[] = {
    {
        .name = "file...",
        .help = "one or more source files",
    },

    { .name = NULL }
};

int
main(int argc, char **argv)
{
    char errbuf[CLP_ERRBUFSZ];
    int idx;
    int rc;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, posparamv, errbuf, &idx);
    if (rc) {
        fprintf(stderr, "%s: %s\n", progname, errbuf);
        exit(rc);
    }

    return 0;
}
`

TODO:
* Since it leverages getopt_long() it is inherently not thread-safe.
