
#include <stdio.h>
#include "clp.h"

bool aflag, bflag, cflag;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_posparam posparamv_a[] = {
    CLP_POSPARAM("[files...]", "zero or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_posparam posparamv_b[] = {
    CLP_POSPARAM("[file]", "zero or one file", NULL, NULL),
    CLP_POSPARAM("right", "right justified positional parameter", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_posparam posparamv_c[] = {
    CLP_POSPARAM("[left]", "optional left justified positional parameter", NULL, NULL),
    CLP_POSPARAM("[files...]", "zero or more files", NULL, NULL),
    CLP_POSPARAM("right", "right justified positional parameter", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION('a', aflag, NULL, "bcx", bool, posparamv_a, "do a stuff..."),
    CLP_OPTION('b', bflag, NULL, "acy", bool, posparamv_b, "do b stuff..."),
    CLP_OPTION('c', cflag, NULL, "abj", bool, posparamv_c, "do c stuff..."),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_posparam *pp = posparamv;
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv, NULL, 0);
    if (rc)
        return rc;

    if (clp_given('a', optionv))
        pp = posparamv_a;
    else if (clp_given('b', optionv))
        pp = posparamv_a;
    else if (clp_given('c', optionv))
        pp = posparamv_c;

    for (int i = 0; i < pp->argc; ++i)
        printf("file[%d] %s\n", i, pp->argv[i]);

    /* do something... */

    return 0;
}
