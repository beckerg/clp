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

clp_posparam_cb posparamv_default_after;
clp_posparam_cb posparamv_list_after;

/* Default positional parameters.
 */
struct clp_posparam posparamv_default[] = {
#if 1
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

#if 1
    {
        .name = "src...",
        .help = "one or more source files",
        .after = posparamv_default_after,
    },
#else
    {
        .name = "dst",
        .help = "destination directory",
        .convert = clp_convert_string, .result = &dst_path,
        .after = posparamv_default_after,
    },

    {
        .name = "[right]",
        .help = "optional right parameter",
        .after = posparamv_default_after,
    },

#endif

#else

    {
        .name = "src...",
        .help = "one or more source files",
        .after = posparamv_default_after,
    },

    {
        .name = "dst",
        .help = "destination directory",
        .convert = clp_convert_string, .result = &dst_path,
        .after = posparamv_default_after,
    },

#endif

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
    CLP_OPTION_HELP,

    // Example of a boolean option that triggers non-default positional parameters.
    {
        .optopt = 'l', .longopt = "list",
        .help = "list...",
        .paramv = posparamv_list,
    },

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
        printf("%s: %s\n", __func__, param->argv[i]);
    }
}

static bool
given(int c)
{
    return !!clp_given(c, optionv, NULL);
}

int
main(int argc, char **argv)
{
    int rc, i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, posparamv_default);
    if (rc)
        return rc;

    printf("progname is %s\n", progname);
    printf("verbosity is %d %d\n", verbosity, given('v'));
    printf("argc=%d optind=%d\n", argc, optind);

    argc -= optind;
    argv += optind;

    for (i = 0; i < argc; ++i) {
        printf("posparams: %d %s\n", i, argv[i]);
    }

    printf("dryrun is %d %d\n", dryrun, given('n'));

    if (cf) {
        printf("cf is %s %d\n", ferror(cf) ? "in error" : "open", given('C'));
    } else {
        printf("cf is nil %d\n", given('C'));
    }

    return 0;
}
