
#include <stdio.h>
#include "clp.h"

bool xflag, yflag;
int verbosity;
int zarg;

struct clp_option optionv[] = {
    CLP_OPTION('x', bool, xflag, "yz", "enable feature x (excludes -yz)"),
    CLP_OPTION('y', bool, yflag, "xz", "enable feature y (excludes -xz)"),
    CLP_OPTION('z', int, zarg, "xy", "enable feature z (excludes -xy)"),
    CLP_OPTION_VERBOSITY(verbosity),
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

    if (clp_given('x', optionv, NULL))
        printf("feature x enabled\n");

    if (clp_given('y', optionv, NULL))
        printf("feature y enabled\n");

    if (clp_given('z', optionv, NULL))
        printf("feature z enabled (zarg=%d)\n", zarg);

    /* do something... */

    return 0;
}
