
#include <stdio.h>
#include <strings.h>
#include <sysexits.h>
#include <getopt.h>

#include "clp.h"

#define SUBCMD(_xname, _xoptionv, _xparamv, _xhelp) \
    { .name = (_xname), .help = (_xhelp), .optionv = (_xoptionv), .posparamv = (_xparamv) }

struct subcmd {
    const char *name;
    const char *help;
    struct clp_option *optionv;
    struct clp_posparam *posparamv;
};

struct subcmd subcmdv[];
struct subcmd *subcmd;
clp_cvt_cb subcmd_cvt;

clp_posparam_cb subcmd_action;
clp_posparam_cb subcmd_after;

char version[] = "1.2.3";
int verbosity, dryrun;
int foo, bar, baz;
char *nvpair;
FILE *fp;

struct clp_posparam posparamv[] = {
    { .name = "cmd...", .help = "subcommand to run",
      .cvtfunc = subcmd_cvt, .cvtparms = subcmdv, .cvtdst = &subcmd, .action = subcmd_action },
    CLP_POSPARAM_END
};
struct clp_option optionv[] = {
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_foo[] = {
    CLP_POSPARAM("files...", fopen, fp, NULL, subcmd_after, "one or more files"),
    CLP_POSPARAM_END
};
struct clp_option optionv_foo[] = {
    CLP_OPTION('c', int, foo, "", "specify foo count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_bar[] = {
    CLP_POSPARAM("[name=value ...]", string, nvpair, NULL, subcmd_after,
                 "zero or more nv pairs"),
    CLP_POSPARAM_END
};
struct clp_option optionv_bar[] = {
    CLP_OPTION('c', int, bar, "", "specify bar count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_baz[] = {
    CLP_POSPARAM("[file]", fopen, fp, NULL, subcmd_after, "zero or one file"),
    CLP_POSPARAM_END
};
struct clp_option optionv_baz[] = {
    CLP_OPTION('c', int, baz, "", "specify baz count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct subcmd subcmdv[] = {
    SUBCMD("foo", optionv_foo, posparamv_foo, "do foo stuff"),
    SUBCMD("bar", optionv_bar, posparamv_bar, "do bar stuff"),
    SUBCMD("baz", optionv_baz, posparamv_baz, "do baz stuff"),
    { .name = NULL }
};

int
subcmd_cvt(struct clp *clp, const char *str, int flags, void *parms, void *dst)
{
    struct subcmd *subcmd = parms;

    for (subcmd = parms; subcmd && subcmd->name; ++subcmd) {
        if (0 == strcasecmp(str, subcmd->name))
            break;
    }

    if (!subcmd || !subcmd->name) {
        clp_eprint(clp, "invalid subcommand '%s', use -h for help", str);
        errno = EINVAL;
        return EX_USAGE;
    }

    *(void **)dst = subcmd;

    return 0;
}

int
subcmd_action(struct clp_posparam *param)
{
    int rc;

    rc = clp_parsev(param->argc, param->argv, subcmd->optionv, subcmd->posparamv);

    return rc ?: -1;
}

int
subcmd_after(struct clp_posparam *param)
{
    for (int i = 0; i < param->argc; ++i)
        printf("%s: %s argv[%d] %s\n", __func__, subcmd->name, i, param->argv[i]);

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

    /* do something... */

    return 0;
}
