
#include <stdio.h>
#include <getopt.h>

#include "clp.h"

int verbosity;

struct clp_posparam posparamv[] = {
    CLP_POSPARAM("files...", "one or more files", NULL, NULL),
    CLP_POSPARAM_END
};

struct clp_option optionv[] = {
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_HELP,
    CLP_OPTION_END
};

int
main(int argc, char **argv)
{
    int rc;

    rc = clp_parsev(argc, argv, optionv, posparamv);
    if (rc)
        return rc;

    if (verbosity > 0) {
        for (int i = optind; i < argc; ++i)
            printf("argv[%d] %s\n", i, argv[i]);

        for (int i = 0; i < posparamv[0].argc; ++i)
            printf("files[%d] %s\n", i, posparamv[0].argv[i]);
    }

    /* do something... */

    return 0;
}
