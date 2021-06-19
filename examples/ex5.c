
#include <stdio.h>
#include "clp.h"

int verbosity;
bool rflag;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("[left]", "optional left justified positional parameter", NULL, NULL),
    CLP_POSPARAM("[middle...]", "zero or more params", NULL, NULL),
    CLP_POSPARAM("right", "right justified positional parameter", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_posparam posparamv_r[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_XOPTION('r', bool, rflag, "^v", "remove files", NULL, NULL, posparamv_r),
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_HELP(),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_posparam *paramv = posparamv, *param;
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv, NULL, 0);
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
