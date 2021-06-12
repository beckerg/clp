#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>

#include "main.h"
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
    CLP_OPTION_VERBOSITY('v', verbosity),
    CLP_OPTION_VERSION('V', version),
    CLP_OPTION_DRYRUN('n', dryrun),
    CLP_OPTION_CONF('C', cf),
    CLP_OPTION_HELP('h'),

    CLP_OPTION('f', myfloat, NULL, NULL, float, NULL, "specify a float"),
    CLP_OPTION('d', mydouble, NULL, NULL, double, NULL, "specify a double"),

    CLP_OPTION('i', myint, NULL, NULL, int, NULL, "specify an int"),
    CLP_OPTION('I', myuint, NULL, NULL, u_int, NULL, "specify a u_int"),

    CLP_OPTION('l', mylong, NULL, NULL, long, NULL, "specify a long"),
    CLP_OPTION('L', myulong, NULL, NULL, u_long, NULL, "specify a u_long"),

    CLP_OPTION('s', mystring, NULL, NULL, string, NULL, "specify a string"),

    { .optopt = 'j', .argname = "intv",
      .cvtfunc = clp_cvt_int, .cvtparms = &intv, .cvtdst = intv.data,
      .help = "specify a vector of ints" },

    // Example of a bunch of mutually exclusive boolean options.
    CLP_OPTION('x', xflag, NULL, "yz", bool, NULL, "specify x flag"),
    CLP_OPTION('y', yflag, NULL, "xz", bool, NULL, "specify y flag"),
    CLP_OPTION('z', zflag, NULL, "xy", bool, NULL, "specify z flag"),

    CLP_OPTION('t', mytime, NULL, NULL, time_t, NULL, "specify a time_t"),
    CLP_OPTION_END
};

static bool
given(int c)
{
    return !!clp_given(c, optionv);
}

int
main(int argc, char **argv)
{
    char errbuf[128];
    int xoptind;
    int rc;
    int i;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    rc = clp_parsev(argc, argv, optionv, NULL, errbuf, sizeof(errbuf), &xoptind);
    if (rc) {
        fprintf(stderr, "%s: %s\n\n", progname, errbuf);
        exit(rc);
    }

    argc -= xoptind;
    argv += xoptind;

    if (argc > 0)
        printf("argc=%d xoptind=%d\n", argc, xoptind);

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
