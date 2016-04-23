#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "clp.h"

char version[] = "version...";
char *progname;
int verbosity;
int dryrun;
FILE *cf;
char *dst_path;

int lcntr, gcntr;

clp_posparam_cb_t posparamv_default_after;
clp_posparam_cb_t posparamv_list_after;

/* Default positional parameters.
 */
clp_posparam_t posparamv_default[] = {
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

    CLP_PARAM_END
};

/* Positional parameters for the list option (-l, --list)
 */
clp_posparam_t posparamv_list[] = {
    {
        .name = "[file...]",
        .help = "zero or more files",
        .after = posparamv_list_after,
    },

    { .name = NULL }
};

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_DRYRUN(dryrun),
    CLP_OPTION_CONF(cf),
    CLP_OPTION_HELP,

	// Example of a boolean option that triggers non-default positional parameters.
    {
        .optopt = 'l', .longopt = "list",
        .help = "list...",
        .paramv = posparamv_list,
    },

    CLP_OPTION(incr, 'L', lcntr, NULL, "increment a local int counter"),
    CLP_OPTION(incr, 'G', gcntr, NULL, "increment a global int counter"),
    
    CLP_OPTION_END
};

void
posparamv_default_after(struct clp_posparam_s *param)
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
posparamv_list_after(struct clp_posparam_s *param)
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
    clp_option_t *opt = clp_option_find(optionv, c);

    return (opt ? opt->given : 0);
}

int
main(int argc, char **argv)
{
    char errbuf[CLP_ERRBUFSZ];
    int optind;
    int i, j;
    int rc;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    for (i = 0; i < 10; ++i) {
        lcntr = 0;

        rc = clp_parsev(argc, argv, optionv, posparamv_default, errbuf, &optind);
        if (rc) {
            fprintf(stderr, "%s: %s\n", progname, errbuf);
            exit(rc);
        }

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

        for (j = argc; j < optind; ++j) {
            printf("posparams: %d %s\n", j, argv[j]);
        }
    }

    return 0;
}
