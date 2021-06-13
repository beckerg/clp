# clp
Command Line Processor

_**clp**_ is command line processor which aims to bring simple consistency
to command line parsing.  It is a one-pass parser/processor, in the sense
that you can simply call _**clp_parsev()**_ or _**clp_parsel()**_ to parse
and process a given argument vector or argument string, respectively.

Other than compiling _**clp.c**_, there are no other external programs
required to generate the parser, nor is there any run-time initialization
needed to "set things up".

_**clp**_ was designed to be embedded directly into an application, but there
should be nothing that prevents one from building it into a shared or static
library.

## Examples
### Example 1 - Simple Options
Here's a very simple example that builds a parser expecting three options and
no positional parameters.  The parameters of the _**CLP_OPTION()**_ macro are:
_option-letter, option-variable, long-name, excludes-string,
conversion-function-suffix, positional-parameters, option-help-string_.

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

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
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
```

By default, integer option argument conversion accepts a single-letter
suffix and modifies the result of the conversion by either the
[IEC](https://en.wikipedia.org/wiki/Binary_prefix)
or
[SI](https://en.wikipedia.org/wiki/Binary_prefix)
multiplier as specified by the first letter of the allowed suffixes
**[kmgtpezyKMGTPEZY]**
(using lower-case for IEC and upper-case for SI suffixes).  You can disable
this behavior by either defining your own option argument conversion functions,
or compiling with **-Dclp_suftab_default=clp_suftab_none**.

Similarly, the `time_t` converter accepts suffixes from the set of
**[smhwdyc]** for seconds, minutes, hours, week, days, years, and centuries.

### Example 2 - Semi-Custom Option Argument Conversion
The simplest way to enforce an upper and lower bound on the _**jobs**_ variable from
example one is to leverage the _**CLP_CVT_XX()**_ macro to generate a named conversion
function with the specified bounds.  Here we use it to instantiate a function named
_**clp_cvt_cvtjobs()**_ which will restrict the value of jobs to the interval [1,10].


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

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
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
line after the options.  Here we define a positional parameter named _**files...**_
which the parser will use to enforce that one or more file names must be given
for any valid invocation of the command.  Note that enclosing _**files...**_ within
square bracets will allow for zero or more files, while removing the ellipsis will
specify exactly one file (or zero-or-one if enclosed in square brackets).

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
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv, NULL, 0);
    if (rc)
        return rc;

    if (verbosity > 0) {
        for (int i = optind; i < argc; ++i)
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
```

Note that we introduced the **-v** option, and that _**getopt(3)'s**_
_**optind**_ indicates where option processing ended.

### Example 4 - Mutually Exclusive Options
A typical tool might often have several options which are mutually exclusive.

```
#include <stdio.h>
#include "clp.h"

bool xflag, yflag;
int verbosity;
int zarg;

struct clp_option optionv[] = {
    CLP_OPTION('x', xflag, NULL, "yz", bool, NULL, "enable feature x (excludes -yz)"),
    CLP_OPTION('y', yflag, NULL, "xz", bool, NULL, "enable feature y (excludes -xz)"),
    CLP_OPTION('z', zarg, NULL, "xy", int, NULL, "enable feature z (excludes -xy)"),
    CLP_OPTION_VERBOSITY('v', verbosity),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
    if (rc)
        return rc;

    if (clp_given('x', optionv))
        printf("feature x enabled\n");

    if (clp_given('y', optionv))
        printf("feature y enabled\n");

    if (clp_given('z', optionv))
        printf("feature z enabled (zarg=%d)\n", zarg);

    /* do something... */

    return 0;
}
```

```
$ ./ex4 -h
usage: ex4 [-v] [-x | -y | -z zarg]
usage: ex4 -h
-h       print this help list
-v       increase verbosity
-x       enable feature x (excludes -yz)
-y       enable feature y (excludes -xz)
-z zarg  enable feature z (excludes -xy)

$ ./ex4 -x
feature x enabled

$ ./ex4 -xy
ex4: option -x excludes -y, use -h for help

$ ./ex4 -y
feature y enabled

$ ./ex4 -y -x
ex4: option -y excludes -x, use -h for help
```

### Example 5 - Complex Positional Parameters
TODO...

## Developer Notes
TODO...

## TODO
* _**clp**_ leverages _getopt_long(3)_ and hence is inherently not thread-safe.
