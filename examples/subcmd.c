
#include <stdio.h>
#include <strings.h>
#include <sysexits.h>
#include <getopt.h>

#include "clp.h"

char version[] = "1.2.3";
int verbosity, dryrun;
int foo, bar, baz;
char *nvpair;
FILE *fp;

struct subcmd {
    const char *name;
    const char *help;
    struct clp_option *optionv;
    struct clp_posparam *posparamv;
};

clp_cvt_cb subcmd_cvt;
struct subcmd *subcmd;

struct clp_posparam posparamv[] = {
    { .name = "cmd", .help = "subcommand to run",
      .cvtfunc = subcmd_cvt, .cvtdst = &subcmd },
    { .name = "[args...]", .help = "subcommand arguments..." },
    CLP_POSPARAM_END
};
struct clp_option optionv[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_foo[] = {
    CLP_POSPARAM("files...", fopen, fp, NULL, "one or more files"),
    CLP_POSPARAM_END
};
struct clp_option optionv_foo[] = {
    CLP_OPTION('c', int, foo, "", "specify foo count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_bar[] = {
    CLP_POSPARAM("[name=value ...]", string, nvpair, NULL, "zero or more nv pairs"),
    CLP_POSPARAM_END
};
struct clp_option optionv_bar[] = {
    CLP_OPTION('c', int, bar, "", "specify bar count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_baz[] = {
    CLP_POSPARAM("[file]", fopen, fp, NULL, "zero or one file"),
    CLP_POSPARAM_END
};
struct clp_option optionv_baz[] = {
    CLP_OPTION('c', int, baz, "", "specify baz count"),
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
subcmd_cvt(struct clp *clp, const char *str, int flags, void *parms, void *dst)
{
    int i;

    for (i = 0; subcmdv[i].name; ++i) {
        if (0 == strcasecmp(str, subcmdv[i].name))
            break;
    }

    if (!subcmdv[i].name) {
        clp_eprint(clp, "invalid subcommand '%s', use -h for help", str);
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
