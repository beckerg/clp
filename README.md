# clp
Command Line Processor

_**clp**_ is command line processor which aims to bring simple consistency
to command line processing.  It is a one-pass parser/processor, in the sense
that you need only call _**clp_parsev()**_ or _**clp_parsel()**_ to process
a given argument vector or argument string, respectively.

Other than compiling _**clp.c**_, there are no other external programs
required to generate the parser, nor is there any run-time initialization
needed to "set things up".

_**clp**_ was designed to be embedded directly into an application, but there
should be nothing that prevents one from building it into a shared or static
library.

## Examples
### Example 1 - Simple Options
Here's a very simple example that builds a parser expecting three options
(-d and -j are explicitly defined, while -h is instantiated via the
_**CLP_OPTION_HELP**_ macro).
Note that since the parser is based upon _**getopt_long(3)**_ there are
many valid ways in which to specify an option on the command line.
Note also that the calls to _**clp_given()**_ are merely illustrative
and not otherwise required.

```
#include <stdio.h>
#include "clp.h"

time_t duration;
int jobs;

struct clp_option optionv[] = {
    CLP_OPTION('d', time_t, duration, "", "specify max duration (seconds)"),
    CLP_OPTION('j', int, jobs, "", "specify max number of jobs"),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    if (clp_given('d', optionv, NULL))
        printf("duration is %ld seconds\n", duration);

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; ++i)
        printf("argv[%d] %s\n", i, argv[i]);

    /* do something... */

    return 0;
}
```

```
$ ./ex1 -h
usage: ex1 [-d duration] [-j jobs] [args...]
usage: ex1 -h
-d duration  specify max duration (seconds)
-h           print this help list
-j jobs      specify max number of jobs
args...  zero or more positional arguments

$ ./ex1 --help
usage: ex1 [-d duration] [-j jobs] [args...]
usage: ex1 -h
-d, --duration duration  specify max duration (seconds)
-h, --help               print this help list
-j, --jobs jobs          specify max number of jobs
args...  zero or more positional arguments

$ ./ex1 -q
ex1: invalid option -q, use -h for help

$ ./ex1 -j
ex1: option -j requires a parameter, use -h for help

$ ./ex1 -j1
jobs is 1

$ ./ex1 --j2
ex1: invalid option --j2, use -h for help

$ ./ex1 --j 3
jobs is 3

$ ./ex1 --jo 4
jobs is 4

$ ./ex1 --jobs 5
jobs is 5

$ ./ex1 --jobs=6
jobs is 6

$ ./ex1 -Wj7
ex1: invalid option -Wj7, use -h for help

$ ./ex1 -Wj 8
jobs is 8

$ ./ex1 -Wjobs=9
jobs is 9

$ ./ex1 -j $((1<<31))
ex1: unable to convert '-j 2147483648': argument not within the interval [INT_MIN, INT_MAX]

$ ./ex1 -j $(((1<<31) - 1))
jobs is 2147483647

$ ./ex1 -j 1k
jobs is 1024

$ ./ex1 -j 1K
jobs is 1000

$ ./ex1 -d3
duration is 3 seconds

$ ./ex1 -d1h
duration is 3600 seconds

$ ./ex1 -j1 a b c
jobs is 1
argv[0] a
argv[1] b
argv[2] c

$ ./ex1

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

Similarly, the default `time_t` converter accepts suffixes from the set of
**[smhwdyc]** for seconds, minutes, hours, week, days, years, and centuries.

### Example 2 - Semi-Custom Option Argument Conversion
The simplest way to enforce an upper and lower bound on the _**jobs**_ variable
from example one is to leverage the _**CLP_CVT_XX()**_ macro to generate a named
conversion function with the specified bounds.  Here we use it to instantiate
a function named _**clp_cvt_jobsfn()**_ which will restrict the value of jobs
to the interval [1,10] if and when the option is given.
Note how we disable the use of option argument suffixes by specifying
_**clp_suftab_none**_ to the conversion template.

```
#include <stdio.h>
#include "clp.h"

int jobs;

CLP_CVT_TMPL(jobsfn, int, 1, 10, clp_suftab_none);

struct clp_option optionv[] = {
    CLP_OPTION('j', jobs, jobsfn, "", "specify max number of jobs"),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
```

```
$ ./ex2 -j0
ex2: unable to convert '-j 0': argument not within the interval [1, 10]

$ ./ex2 -j1
jobs is 1

$ ./ex2 -j10
jobs is 10

$ ./ex2 -j11
ex2: unable to convert '-j 11': argument not within the interval [1, 10]

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
Note that we introduce the **-v** option, and show that _**getopt_long(3)'s**_
_**optind**_ indicates where option processing ended.

```
#include <stdio.h>
#include <getopt.h>
#include "clp.h"

int verbosity;
FILE *fp;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("files...", fopen, fp, "one or more files");
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv);
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


### Example 4 - Mutually Exclusive Options
A typical tool might often have several options which are mutually exclusive.

```
#include <stdio.h>
#include "clp.h"

bool xflag, yflag;
int verbosity;
int zarg;

struct clp_option optionv[] = {
    CLP_OPTION('x', bool, xflag, "yz", "enable feature x (excludes -yz)"),
    CLP_OPTION('y', bool, yflag, "xz", "enable feature y (excludes -xz)"),
    CLP_OPTION('z', int, zarg, "xy", "enable feature z (excludes -xy)"),
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    if (clp_given('x', optionv, NULL))
        printf("feature x enabled\n");

    if (clp_given('y', optionv, NULL))
        printf("feature y enabled\n");

    if (clp_given('z', optionv, NULL))
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
This example illustrate a more complicated arrangement of positional parameters,
with an option (-r) that has a different posparam syntax than the default.

```
#include <stdio.h>
#include "clp.h"

int left, middle, right;
int verbosity;
bool rflag;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("[left]", int, left, "optional left justified positional parameter"),
    CLP_POSPARAM("[middle...]", int, middle, "zero or more params"),
    CLP_POSPARAM("right", int, right, "right justified positional parameter"),
    CLP_POSPARAM_END
};

struct clp_posparam posparamv_r[] = {
    CLP_POSPARAM("files...", int, left, "one or more files"),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_XOPTION('r', bool, rflag, "^v", "remove files", NULL, NULL, posparamv_r),
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_posparam *paramv = posparamv, *param;
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv);
    if (rc)
        return rc;

    if (clp_given('r', optionv, NULL))
        paramv = posparamv_r;

    for (param = paramv; param->name; ++param) {
        for (int i = 0; i < param->argc; ++i)
            printf("%s[%d] %s\n", param->name, i, param->argv[i]);
    }

    /* do something... */

    return 0;
}
```

```
$ ./examples/ex5 -h
usage: ex5 [-v] [left [middle...]] right
usage: ex5 -h
usage: ex5 -r [-v] files...
-h  print this help list
-r  remove files
-v  increase verbosity
left       optional left justified positional parameter
middle...  zero or more params
right      right justified positional parameter
files...   one or more files

$ ./examples/ex5 a
right[0] a

$ ./examples/ex5 a b
[left][0] a
right[0] b

$ ./examples/ex5 a b c
[left][0] a
[middle...][0] b
right[0] c

$ ./examples/ex5 a b c d
[left][0] a
[middle...][0] b
[middle...][1] c
right[0] d

$ ./examples/ex5 -r a b c
files...[0] a
files...[1] b
files...[2] c
```

### Example 6 - Thread-local variables
The following example illustrates how to specify that the option
option arguments reside in **thread-local**, **stack**, or
**dynamically allocated memory**.  In these cases _**clp**_
will stash the converted option argument into a private buffer
from which it may be obtained by calling _***clp_given()***_.

```
#include <stdio.h>
#include "clp.h"

static __thread time_t duration;

static __thread struct clp_option optionv[] = {
    CLP_OPTION_TLS('d', time_t, duration, NULL, NULL, "specify max duration (seconds)"),
    CLP_OPTION_TLS('j', int, jobs, NULL, NULL, "specify max number of jobs"),
    CLP_OPTION_TLS('C', fp, conf, NULL, NULL, "specify config file"),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_option *o;
    int *jobs;
    FILE *fp;
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    o = clp_given('d', optionv, &duration);
    if (o)
        printf("duration is %ld seconds\n", duration);

    o = clp_given('C', optionv, &fp);
    if (o)
        printf("conf fp is %p, fd %d\n", fp, fileno(fp));

    jobs = calloc(1, sizeof(*jobs));
    if (jobs) {
        o = clp_given('j', optionv, jobs);
        if (o)
            printf("jobs is %d\n", *jobs);

        free(jobs);
    }

    /* do something... */

    return 0;
}
```

### Example 7 - Subcommands
_**clp**_ makes it easy to build a parser that supports one or more
subcommands.  See examples/subcmd.c for the full example, but here
are required bits:

```
struct clp_posparam posparamv_foo[] = {
    CLP_POSPARAM("files...", fopen, fp, NULL, do_foo, "one or more files"),
    CLP_POSPARAM_END
};

struct clp_subcmd subcmdv[] = {
    CLP_SUBCMD("foo", optionv_foo, posparamv_foo, "do foo stuff"),
    CLP_SUBCMD("bar", optionv_bar, posparamv_bar, "do bar stuff"),
    CLP_SUBCMD("baz", optionv_baz, posparamv_baz, "do baz stuff"),
    CLP_SUBCMD("hidden", optionv, clp_posparam_none, NULL),
    CLP_SUBCMD_END
};

struct clp_posparam posparamv[] = {
    CLP_POSPARAM_SUBCMD("cmd", subcmdv, &subcmd, NULL),
    { .name = "[args...]", .help = "subcommand arguments" },
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

int
do_foo(struct clp_posparam *param)
{
    if (clp_given('v', subcmd->optionv, NULL))
        printf("%s: verbosity is %d\n", subcmd->name, verbosity);

    for (int i = 0; i < param->argc; ++i)
        printf("%s: argv[%d] %s\n", subcmd->name, i, param->argv[i]);

    return 0;
}

int
main(int argc, char **argv)
{
    return clp_parsev(argc, argv, optionv, posparamv);
}
```


$ ./examples/subcmd
subcmd: 1 positional argument required, use -h for help

$ ./examples/subcmd -h
usage: subcmd [-nv] cmd [args...]
usage: subcmd -h
usage: subcmd -V
-h  print this help list
-n  dry run
-V  print version
-v  increase verbosity
cmd  one of {foo, bar, baz}
  foo  do foo stuff
  bar  do bar stuff
  baz  do baz stuff
args...  subcommand arguments

$ ./examples/subcmd foo
foo: 1 positional argument required, use -h for help

$ ./examples/subcmd foo -h
usage: foo [-nv] [-c foo] files...
usage: foo -h
usage: foo -V
-c foo  specify foo count
-h      print this help list
-n      dry run
-V      print version
-v      increase verbosity
files...  one or more files

$ ./examples/subcmd foo -vvv LICENSE
foo: verbosity is 3
foo: argv[0] LICENSE

$ ./examples/subcmd ba
subcmd: ambiguous subcommand 'ba', use -h for help


## Developer Notes
_**clp**_ always builds a "posixly correct" parser, and requires that all options
have a single-letter option allowed by _**isgraph(3)**_ excluding the **':'**
and **'?'** characters as the latter are produced by _**getopt_long(3)**_.

## TODO
* _**clp**_ leverages _**getopt_long(3)**_ and hence is inherently not thread-safe
nor re-entrant.
