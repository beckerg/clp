
#if __linux__
#define _DEFAULT_SOURCE
#endif

#include <stdio.h>
#include "clp.h"

static __thread time_t duration;

static __thread struct clp_option optionv[] = {
    CLP_OPTION_TLS('d', time_t, duration, NULL, "specify max duration (seconds)"),
    CLP_OPTION_TLS('j', int, jobs, NULL, "specify max number of jobs"),
    CLP_OPTION_TLS('C', fp, conf, NULL, "specify config file"),
    CLP_OPTION_HELP(),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    struct clp_option *o;
    int *jobs;
    FILE *fp;
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    o = clp_given('d', optionv, &duration);
    if (o)
        printf("duration is %ld seconds\n", duration);

    o = clp_given('C', optionv, &fp);
    if (o)
        printf("conf fp is %p, fd %d\n", fp, fileno(fp));

    jobs = calloc(1, sizeof(*jobs));
    if (jobs) {
        o = clp_given('j', optionv, jobs);
        if (o)
            printf("jobs is %d\n", *jobs);

        free(jobs);
    }

    /* do something... */

    return 0;
}
