
#include <stdio.h>
#include <getopt.h>
#include <sysexits.h>

#include "clp.h"

char version[] = "1.2.3";
int verbosity, dryrun;

struct subcmd {
    const char *name;
    const char *help;
    struct clp_option *optionv;
    struct clp_posparam *posparamv;
};

clp_cvt_cb subcmd_cvt;
struct subcmd *subcmd;

struct clp_posparam posparamv[] = {
    { .name = "cmd...", .help = "subcommand to run",
      .cvtfunc = subcmd_cvt, .cvtdst = &subcmd },
    CLP_POSPARAM_END
};
struct clp_option optionv[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_foo[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};
struct clp_option optionv_foo[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_bar[] = {
    CLP_POSPARAM("[files...]", "zero or more files", NULL, NULL),
    CLP_POSPARAM_END
};
struct clp_option optionv_bar[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_baz[] = {
    CLP_POSPARAM("[file]", "zero or one file", NULL, NULL),
    CLP_POSPARAM_END
};
struct clp_option optionv_baz[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct subcmd subcmdv[] = {
    { .name = "foo", .help = "do foo action...",
      .optionv = optionv_foo, .posparamv = posparamv_foo },
    { .name = "bar", .help = "do bar action...",
      .optionv = optionv_bar, .posparamv = posparamv_bar },
    { .name = "baz", .help = "do baz action...",
      .optionv = optionv_baz, .posparamv = posparamv_baz },
    { .name = NULL }
};

int
subcmd_cvt(const char *str, int flags, void *parms, void *dst)
{
    int i;

    for (i = 0; subcmdv[i].name; ++i) {
        if (0 == strcasecmp(str, subcmdv[i].name))
            break;
    }

    if (!subcmdv[i].name) {
        errno = EINVAL;
        return EX_USAGE;
    }

    *(void **)dst = subcmdv + i;

    return -1;
}

int
main(int argc, char **argv)
{
    int rc, i;

    rc = clp_parsev(argc, argv, optionv, posparamv);
    if (rc)
        return rc;

    if (clp_given('V', optionv, NULL))
        return 0;

    if (clp_given('h', optionv, NULL)) {
        for (i = 0; subcmdv[i].name; ++i) {
            printf("%-4s  %s\n", subcmdv[i].name, subcmdv[i].help);
        }
        printf("\n");
        return 0;
    }

    argc -= optind;
    argv += optind;

    rc = clp_parsev(argc, argv, subcmd->optionv, subcmd->posparamv);
    if (rc)
        return rc;

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; ++i)
        printf("argv[%d] %s\n", i, argv[i]);

    /* do something... */

    return 0;
}
