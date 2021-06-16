
#include <stdio.h>
#include "clp.h"

time_t duration;
int jobs;

struct clp_option optionv[] = {
    CLP_OPTION('d', time_t, duration, NULL, NULL, "specify max duration (seconds)"),
    CLP_OPTION('j', int, jobs, NULL, NULL, "specify max number of jobs"),
    CLP_OPTION_HELP(),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
    if (rc)
        return rc;

    if (clp_given('d', optionv, NULL))
        printf("duration is %ld seconds\n", duration);

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    /* do something... */

    return 0;
}
