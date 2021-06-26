
#include <stdio.h>
#include <strings.h>
#include <sysexits.h>
#include <getopt.h>

#include "clp.h"

struct clp_option optionv[];
struct clp_subcmd *subcmd;
clp_posparam_cb do_cmd;

char version[] = "1.2.3";
int verbosity, dryrun;
int foo, bar, baz;
char *nvpair;
FILE *fp;

struct clp_posparam posparamv_foo[] = {
    CLP_POSPARAM("files...", fopen, fp, NULL, do_cmd, "one or more files"),
    CLP_POSPARAM_END
};
struct clp_option optionv_foo[] = {
    CLP_OPTION('c', int, foo, "", "specify foo count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_bar[] = {
    CLP_POSPARAM("[name=value ...]", string, nvpair, NULL, do_cmd,
                 "zero or more nv pairs"),
    CLP_POSPARAM_END
};
struct clp_option optionv_bar[] = {
    CLP_OPTION('c', int, bar, "", "specify bar count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
};

struct clp_posparam posparamv_baz[] = {
    CLP_POSPARAM("[file]", fopen, fp, NULL, do_cmd, "zero or one file"),
    CLP_POSPARAM_END
};
struct clp_option optionv_baz[] = {
    CLP_OPTION('c', int, baz, "", "specify baz count"),
    CLP_OPTION_STD(verbosity, version, dryrun),
    CLP_OPTION_END
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
do_cmd(struct clp_posparam *param)
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
