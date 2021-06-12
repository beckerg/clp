
#include <stdio.h>
#include "clp.h"

int jobs;

CLP_CVT_TMPL(cvtjobs, int, 1, 10, clp_suftab_none);

struct clp_option optionv[] = {
    CLP_OPTION('j', jobs, "jobs", NULL, cvtjobs, NULL, "specify max number of jobs"),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0, NULL);
    if (rc)
        return rc;

    if (clp_given('j', optionv))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
