
#include <stdbool.h>
#include "clp.h"

int jobs;
bool xflag, yflag;

CLP_CVT_XX(cvtjobs, int, 1, 10, clp_suftab_none);

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION('j', jobs, "jobs", NULL, cvtjobs, NULL, "specify max number of jobs"),
    CLP_OPTION('x', xflag, NULL, "y", bool, NULL, "enable feature x (excludes -y)"),
    CLP_OPTION('y', yflag, NULL, "x", bool, NULL, "enable feature y (excludes -x)"),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv, NULL, 0, NULL);
    if (rc)
        return rc;

    if (clp_given('x', optionv))
        printf("feature x enabled\n");

    if (clp_given('y', optionv))
        printf("feature y enabled\n");

    /* do something... */

    return 0;
}
