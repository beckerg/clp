
#include <stdio.h>
#include "clp.h"

int verbosity, dryrun, jobs;
time_t duration;
bool xarg, zarg;

struct clp_option optionv[] = {
    CLP_OPTION('d', time_t, duration, NULL, NULL, "specify max duration (seconds)"),
    CLP_OPTION('j', int, jobs, NULL, NULL, "specify max number of jobs"),
    CLP_OPTION('x', bool, xarg, NULL, NULL, NULL),
    CLP_OPTION('z', bool, zarg, "djx", NULL, "zflag (excludes -d and -j)"),
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_DRYRUN(dryrun),
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

    if (clp_given('x', optionv, NULL))
        printf("secret expert mode enabled\n");

    if (clp_given('z', optionv, NULL))
        printf("z mode enabled\n");

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; ++i)
        printf("argv[%d] %s\n", i, argv[i]);

    /* do something... */

    return 0;
}
