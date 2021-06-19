
#include <stdio.h>
#include "clp.h"

char version[] = "1.2.3";
int verbosity, dryrun, jobs;
time_t duration;
bool xarg, zarg;

struct clp_option optionv[] = {
    CLP_OPTION('d', time_t, duration, NULL, "specify max duration (seconds)"),
    CLP_OPTION('j', int, jobs, NULL, "specify max number of jobs"),
    CLP_OPTION('x', bool, xarg, NULL, NULL),
    CLP_OPTION('z', bool, zarg, "djx", "zflag (excludes -d and -j)"),
    { .optopt = 'q', .help = "q opt" },
    { .optopt = 'p', .argname = "popt", .help = "p opt" },
    { .optopt = 1, .argname = "goof", .help = "invalid option letter" },
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_DRYRUN(dryrun),
    CLP_OPTION_HELP(),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_option *o;
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
    if (rc)
        return rc;

    if (clp_given('h', optionv, NULL))
        printf("help was given\n");

    if (clp_given('d', optionv, NULL))
        printf("duration is %ld seconds\n", duration);

    if (clp_given('j', optionv, NULL))
        printf("jobs is %d\n", jobs);

    if (clp_given('x', optionv, NULL))
        printf("secret expert mode enabled\n");

    if (clp_given('v', optionv, NULL))
        printf("verbosity is %d\n", verbosity);

    if (clp_given('z', optionv, NULL))
        printf("z mode enabled\n");

    o = clp_given('q', optionv, NULL);
    if (o)
        printf("q %d\n", o->given);

    o = clp_given('p', optionv, NULL);
    if (o)
        printf("q %s\n", o->optarg);

    argc -= optind;
    argv += optind;

    for (int i = 0; i < argc; ++i)
        printf("argv[%d] %s\n", i, argv[i]);

    /* do something... */

    return 0;
}
