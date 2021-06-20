#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include <getopt.h>

#include "clp.h"

char version[] = "version...";
char *progname;
int verbosity;
int dryrun;
FILE *cf;
int myint;
unsigned int myuint;
long mylong;
unsigned long myulong;
float myfloat;
double mydouble;
char *mystring = "default";
bool xflag = false;
bool yflag = false;
bool zflag = true;
time_t mytime;


CLP_VECTOR(intv, int, 5, ",");

struct clp_option optionv[] = {
    CLP_OPTION_VERBOSITY(verbosity),
    CLP_OPTION_VERSION(version),
    CLP_OPTION_DRYRUN(dryrun),
    CLP_OPTION_CONF(cf),
    CLP_OPTION_HELP(),

    CLP_OPTION('f', float, myfloat, "", "specify a float"),
    CLP_OPTION('d', double, mydouble, "", "specify a double"),

    CLP_OPTION('i', int, myint, "", "specify an int"),
    CLP_OPTION('I', u_int, myuint, "", "specify a u_int"),

    CLP_OPTION('l', long, mylong, "", "specify a long"),
    CLP_OPTION('L', u_long, myulong, NULL, "specify a u_long"),

    CLP_OPTION('s', string, mystring, NULL, "specify a string"),

    { .optopt = 'j', .argname = "intv",
      .cvtfunc = clp_cvt_int, .cvtparms = &intv, .cvtdst = intv.data,
      .help = "specify a vector of ints" },

    // Example of a bunch of mutually exclusive boolean options.
    CLP_OPTION('x', bool, xflag, "yz", "specify x flag"),
    CLP_OPTION('y', bool, yflag, "xz", "specify y flag"),
    CLP_OPTION('z', bool, zflag, "xy", "specify z flag"),

    CLP_OPTION('t', time_t, mytime, NULL, "specify a time_t"),
    CLP_OPTION_END
};

static bool
given(int c)
{
    return !!clp_given(c, optionv, NULL);
}

int
main(int argc, char **argv)
{
    int rc, i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL);
    if (rc)
        return rc;

    argc -= optind;
    argv += optind;

    if (argc > 0)
        printf("argc=%d optind=%d\n", argc, optind);

    if (given('v'))
        printf("verbosity is %d %d\n", verbosity, given('v'));

    for (i = 0; i < argc; ++i) {
        printf("posparams: %d %s\n", i, argv[i]);
    }

    if (given('n'))
        printf("dryrun is %d %d\n", dryrun, given('n'));

    if (cf || given('C'))
        printf("cf is %s %d\n", ferror(cf) ? "in error" : "open", given('C'));

    if (given('i'))
        printf("myint is %d %d\n", myint, given('i'));

    if (given('I'))
        printf("myuint is %u %d\n", myuint, given('I'));

    if (given('l'))
        printf("mylong is %ld %d\n", mylong, given('l'));

    if (given('L'))
        printf("myulong is %lu %d\n", myulong, given('L'));

    if (given('s'))
        printf("mystring is %s %d\n", mystring, given('s'));

    if (given('x'))
        printf("x is %s %d\n", xflag ? "true" : "false", given('x'));

    if (given('y'))
        printf("y is %s %d\n", yflag ? "true" : "false", given('y'));

    if (given('z'))
        printf("z is %s %d\n", zflag ? "true" : "false", given('z'));

    if (given('f'))
        printf("f is %.*g\n", FLT_DECIMAL_DIG, myfloat);

    if (given('d'))
        printf("d is %.*lf\n", DBL_DECIMAL_DIG, mydouble);

    if (given('j')) {
        printf("intv is: ");
        for (i = 0; i < intv.len; ++i) {
            printf(" %d", intv.data[i]);
        }
        printf("\n");
    }

    if (given('t'))
        printf("t is %ld\n", mytime);

    printf("\n");

    return 0;
}
