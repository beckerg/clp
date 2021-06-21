
#include <stdio.h>
#include <getopt.h>

#include "clp.h"

time_t duration;
int jobs;

struct clp_option optionv[] = {
    CLP_OPTION('d', time_t, duration, NULL, "specify max duration (seconds)"),
    CLP_OPTION('j', int, jobs, NULL, "specify max number of jobs"),
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

    if (clp_given('d', optionv, NULL))
        printf("duration is %ld seconds\n", duration);

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; ++i)
        printf("argv[%d] %s\n", i, argv[i]);

    /* do something... */

    return 0;
}
