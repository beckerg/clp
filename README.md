# clp
Command Line Processor

clp is (yet another) command line processor which aims to bring consistency and simplicity
to command line parsing.  It is a one-pass parser/processor, in the sense that you can
simply call clp_parsev() to parse and process a given argument vector.

You can either link your application with libclp, or simply compile clp.c/clp.h directly
into your application.  There are no external programs to run to generate the parser,
nor are there any functions to call to set things up (e.g., some tools of this ilk
require creating an object for each option).

By default, clp builds as a static library, but your best bet is to simply
include clp/lib/clp.[ch] into your project.

## Examples
### Options
Here's a simple example that builds a parser which requires one or more positional
parameters, accepts an integer valued option -j to specify the number of jobs,
accepts two boolean options -x and -y which are mutually exclusive, and accepts
several canned options (e.g., -h for help, ...).  Note that a program can simply
pass NULL to clp_parse() for the option and/or positional parameter arguments
if one or the other is not required.

```
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "clp.h"

char version[] = "1.0.1";
char *progname;
int verbosity;
u_int jobs;
bool xvar, yvar;

struct clp_option optionv[] = {
    CLP_OPTION(u_int, 'j', jobs, "jobs", NULL, "specify max number of jobs"),
    CLP_OPTION(bool, 'x', xvar, NULL, "y", "enable x feature (excludes -y)"),
    CLP_OPTION(bool, 'y', yvar, NULL, "x", "enable y feature (excludes -x)"),
    CLP_OPTION_VERBOSE(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

struct clp_posparam posparamv[] = {
    { .name = "[left...]", .help = "one or more optional left-justified parameters", },
    { .name = "right", .help = "mandatory right-justified parameter", },
    CLP_PARAM_END
};

int
main(int argc, char **argv)
{
    char errbuf[128];
    int xoptind, rc;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, posparamv, errbuf, sizeof(errbuf), &xoptind);
    if (rc) {
        fprintf(stderr, "%s: %s\n\n", progname, errbuf);
        exit(rc);
    }

    if (verbosity > 0) {
        struct clp_posparam *right = &posparamv[1];
        struct clp_posparam *left = &posparamv[0];

        if (right->argc > 0) {
            for (int i = 0; i < left->argc; ++i)
                printf("left param %d: %s\n", i, left->argv[i]);

            printf("right param: %s\n", right->argv[0]);
        }
    }

    return 0;
}
```

```
$ ./a.out
a.out: mandatory positional parameters required, use -h for help

$ ./a.out -h
usage: a.out [-v] [-j jobs] [-x | -y] [left...] right
usage: a.out -h [-v]
usage: a.out -V
-h       print this help list
-j jobs  specify max number of jobs
-V       print version
-v       increase verbosity
-x       enable x feature (excludes -y)
-y       enable y feature (excludes -x)
left...  one or more optional left-justified parameters
right    mandatory right-justified parameter

$ ./a.out a

$ ./a.out -v a
right param: a

$ ./a.out -v a b
left param 0: a
right param: b

$ ./a.out -v a b c
left param 0: a
left param 1: b
right param: c

$ ./a.out -x -y
a.out: option -x excludes -y, use -h for help
```

## TODO
* Since it leverages getopt_long() it is inherently not thread-safe.
* Usage output for mutually exclusive options not always correct.
