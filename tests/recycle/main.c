#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "clp.h"

char version[] = "version...";
char *progname;
int verbosity;
int dryrun;
FILE *cf;
char *dst_path;

int lcntr, gcntr;
char *mystring = "default is non-null";

clp_posparam_cb posparamv_default_after;
clp_posparam_cb posparamv_list_after;

/* Default positional parameters.
 */
struct clp_posparam posparamv_default[] = {
    {
        .name = "[leftmost]",
        .help = "optional left-most parameter",
        .after = posparamv_default_after,
    },

    {
        .name = "[left]",
        .help = "optional left parameter",
        .after = posparamv_default_after,
    },

    {
        .name = "src...",
        .help = "one or more source files",
        .after = posparamv_default_after,
    },

    CLP_POSPARAM_END
};

/* Positional parameters for the list option (-l, --list)
 */
struct clp_posparam posparamv_list[] = {
    {
        .name = "[file...]",
        .help = "zero or more files",
        .after = posparamv_list_after,
    },

    { .name = NULL }
};

struct clp_option optionv[] = {
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_DRYRUN(dryrun),
    CLP_OPTION_CONF(cf),
    CLP_OPTION_HELP(),

    // Example of a boolean option that triggers non-default positional parameters.
    {
        .optopt = 'l', .longopt = "list",
        .help = "list...",
        .paramv = posparamv_list,
    },

    CLP_OPTION('L', incr, lcntr, "", "increment a local int counter"),
    CLP_OPTION('G', incr, gcntr, "", "increment a global int counter"),
    CLP_OPTION('s', string, mystring, "", "specify a string"),

    CLP_OPTION_END
};

void
posparamv_default_after(struct clp_posparam *param)
{
    int i;

    if (param->argc == 0) {
        printf("%s: name=%-12s  unfilled\n",
               __func__, param->name);
    }

    for (i = 0; i < param->argc; ++i) {
        printf("%s: name=%-12s  argv[%d]=%s\n",
               __func__, param->name, i, param->argv[i]);
    }
}

void
posparamv_list_after(struct clp_posparam *param)
{
    int i;

    for (i = 0; i < param->argc; ++i) {
        printf("%s: name=%-12s  argv[%d]=%s\n",
               __func__, param->name, i, param->argv[i]);
    }
}

static int
given(int c)
{
    struct clp_option *opt = clp_given(c, optionv, NULL);

    return (opt ? opt->given : 0);
}

int
main(int argc, char **argv)
{
    int rc, i, j;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    for (i = 0; i < 10; ++i) {
        lcntr = 0;

        rc = clp_parsev(argc, argv, optionv, posparamv_default);
        if (rc)
           return rc;

        if (given('v'))
            printf("verbosity is %d %d\n", verbosity, given('v'));

        if (given('n'))
            printf("dryrun is %d %d\n", dryrun, given('n'));

        if (given('C')) {
            if (cf) {
                printf("cf is %s %d\n", ferror(cf) ? "in error" : "open", given('C'));
            } else {
                printf("cf is nil %d\n", given('C'));
            }
        }

        if (given('L'))
            printf("lcntr is %d %d\n", lcntr, given('L'));

        if (given('G'))
            printf("gcntr is %d %d\n", gcntr, given('G'));

        if (given('s'))
            printf("mystring is %s %d\n", mystring, given('s'));

        for (j = argc; j < optind; ++j) {
            printf("posparams: %d %s\n", j, argv[j]);
        }
    }

    return 0;
}
