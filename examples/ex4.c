
#include <stdio.h>
#include "clp.h"

bool xflag, yflag;
int verbosity;
int zarg;

struct clp_option optionv[] = {
    CLP_OPTION('x', xflag, NULL, "yz", bool, NULL, "enable feature x (excludes -yz)"),
    CLP_OPTION('y', yflag, NULL, "xz", bool, NULL, "enable feature y (excludes -xz)"),
    CLP_OPTION('z', zarg, NULL, "xy", int, NULL, "enable feature z (excludes -xy)"),
    CLP_OPTION_VERBOSITY('v', verbosity),
    CLP_OPTION_HELP('h'),
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, NULL, NULL, 0);
    if (rc)
        return rc;

    if (clp_given('x', optionv))
        printf("feature x enabled\n");

    if (clp_given('y', optionv))
        printf("feature y enabled\n");

    if (clp_given('z', optionv))
        printf("feature z enabled (zarg=%d)\n", zarg);

    /* do something... */

    return 0;
}
