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

## Example
Here's a simple example that builds a parser which accepts the integer valued
option -j to specify the number of jobs, two integer valued options -x and -y
which are mutually exclusive, and several canned options (e.g., -h for help, ...)

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clp.h"

char version[] = "1.0";
char *progname;
int verbosity;
u_int jobs;
int xvar, yvar;

struct clp_option optionv[] = {
    CLP_OPTION(u_int, 'j', jobs, "jobs", NULL, "specify max number of jobs"),
    CLP_OPTION(int, 'x', xvar, NULL, "y", "specify x (excludes -y)"),
    CLP_OPTION(int, 'y', yvar, NULL, "x", "specify y (excludes -x)"),
    CLP_OPTION_VERBOSE(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    char errbuf[128];
    int xoptind, rc;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL, errbuf, sizeof(errbuf), &xoptind);
    if (rc) {
        fprintf(stderr, "%s: %s\n\n", progname, errbuf);
        exit(rc);
    }

    argc -= xoptind;
    argv += xoptind;

    /* do stuff... */

    return 0;
}
```

```
$ ./a.out -h
usage: a.out [-v] [-j jobs] [-x | -y]
usage: a.out -h [-v]
usage: a.out -V
-h       print this help list
-j jobs  specify max number of jobs
-V       print version
-v       increase verbosity
-x xvar  specify x (excludes -y)
-y yvar  specify y (excludes -x)

$ ./a.out --help
usage: a.out [-v] [-j jobs] [-x | -y]
usage: a.out -h [-v]
usage: a.out -V
-h, --help       print this help list
-j, --jobs jobs  specify max number of jobs
-V, --version    print version
-v               increase verbosity
-x xvar          specify x (excludes -y)
-y yvar          specify y (excludes -x)

$ ./a.out -j7 -x1 -y2
a.out: option -x excludes -y, use -h for help
```

TODO:
* Since it leverages getopt_long() it is inherently not thread-safe.
