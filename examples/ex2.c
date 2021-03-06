
#define _GNU_SOURCE

#include <stdio.h>
#include <getopt.h>

#include "clp.h"

int jobs;

CLP_CVT_TMPL(jobs, int, 1, 10, clp_suftab_none);

struct clp_option optionv[] = {
    CLP_OPTION('j', jobs, jobs, NULL, "specify max number of jobs"),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
