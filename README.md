# clp
Command Line Processor

clp is (yet another) command line processor which aims to bring consistency
and simplicity command line parsing.  It is a one-pass parser/processor, in
the sense that you can simply call clp_parsev() to parse and process a given
argument vector.

Other than simply compiling clp.c, there are no other external programs
required to generate the parser, nor is there any run-time initialization
needed to "set things up".

clp was designed to be embedded directly into an application, but there
is nothing that prevents one from building a shared or static library.

## Examples
### Example 1 - Simple Options
Here's a simple example that builds a parser expecting three options and
no positional parameters.

```
#include <stdio.h>
#include "clp.h"

time_t duration;
int jobs;

struct clp_option optionv[] = {
    CLP_OPTION('d', duration, "duration", NULL, time_t, NULL, "specify max duration (seconds)"),
    CLP_OPTION('j', jobs, "jobs", NULL, int, NULL, "specify max number of jobs"),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0, NULL);
    if (rc)
        return rc;

    if (clp_given('d', optionv))
        printf("duration is %ld seconds\n", duration);

    if (clp_given('j', optionv))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
```

```
$ ./ex1 -h
usage: ex1 [-d duration] [-j jobs]
usage: ex1 -h
-d duration  specify max duration (seconds)
-h           print this help list
-j jobs      specify max number of jobs

$ ./ex1 --help
usage: ex1 [-d duration] [-j jobs]
usage: ex1 -h
-d, --duration duration  specify max duration (seconds)
-h, --help               print this help list
-j, --jobs jobs          specify max number of jobs

$ ./ex1 -q
ex1: invalid option -q, use -h for help

$ ./ex1 -j
ex1: option -j requires a parameter, use -h for help

$ ./ex1 -j1
jobs is 1

$ ./ex1 -j $((1<<31))
ex1: unable to convert '-j 2147483648': Numerical argument out of domain

$ ./ex1 -j $(((1<<31) - 1))
jobs is 2147483647

$ ./ex1 -j 1k
jobs is 1024

$ ./ex1 -j 1K
jobs is 1000

$ ./ex1 -d1
duration is 1 seconds

$ ./ex1 -d1h
duration is 3600 seconds

By default, integer option argument conversion accepts a single-letter suffix
and modifies the result of the conversion by either the IEC or SI multiplier
as specified by the first letter of the allowed suffixes [kmgtpezyKMGTPEZY]
(using lower-case for IEC and upper-case for SI suffixes).  Similarly, the
time_t converter accepts suffixes from the set of [smhwdyc] for seconds,
minutes, hours, days, years, and centuries.  You can disable this behavior
by either defining your own option argument conversion functions, or
compiling with -Dclp_suftab_default=clp_suftab_none.
```

### Example 2 - Semi-Custom Option Argument Conversion
The simplest way to enforce an upper and lower bound on the jobs variable from
example one is to leverage the CLP_CVT_XX() macro to generate a named conversion
function with the specified bounds.  Here we use it to define a function named
"clp_cvt_cvtjobs()" which will restrict the value of jobs to the interval [1,10].


```
#include <stdio.h>
#include "clp.h"

int jobs;

CLP_CVT_TMPL(cvtjobs, int, 1, 10, clp_suftab_none);

struct clp_option optionv[] = {
    CLP_OPTION('j', jobs, "jobs", NULL, cvtjobs, NULL, "specify max number of jobs"),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0, NULL);
    if (rc)
        return rc;

    if (clp_given('j', optionv))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
```

```
$ ./ex2 -j0
ex2: unable to convert '-j 0': Numerical argument out of domain

$ ./ex2 -j1
jobs is 1

$ ./ex2 -j10
jobs is 10

$ ./ex2 -j11
ex2: unable to convert '-j 11': Numerical argument out of domain

$ ./ex2 -j4k
ex2: unable to convert '-j 4k': Invalid argument
```

### Example 3 - Simple Positional Parameters
A typical tool will often require one or more arguments to be passed on the command
line after the options.  Here we define a positional parameter named "files..."
which the parser will use to enforce that one or more files must be given for any
valid invocation of the command.

```
#include <stdio.h>
#include "clp.h"

int verbosity;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION_VERBOSITY('v', verbosity),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int xoptind;
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv, NULL, 0, &xoptind);
    if (rc)
        return rc;

    if (verbosity > 0) {
        for (int i = xoptind; i < argc; ++i)
            printf("argv[%d] %s\n", i, argv[i]);

        for (int i = 0; i < posparamv[0].argc; ++i)
            printf("files[%d] %s\n", i, posparamv[0].argv[i]);
    }

    /* do something... */

    return 0;
}
```

```
$ ./ex3
ex3: mandatory positional parameters required, use -h for help

$ ./ex3 -h
usage: ex3 [-v] files...
usage: ex3 -h
-h  print this help list
-v  increase verbosity
files...  one or more files

$ ./ex3 foo

$ ./ex3 -v foo
argv[2] foo
file[0] foo

$ ./ex3 -v foo bar baz
argv[2] foo
argv[3] bar
argv[4] baz
files[0] foo
files[1] bar
files[2] baz

Note that we introduced the -v option, and that clp_parsev() returns xoptind
to indicate where option processing ended (much like how getopt() sets the
global variable optind).
```


## TODO
* Since it leverages getopt_long() it is inherently not thread-safe.
