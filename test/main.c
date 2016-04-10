/*
 * Copyright (c) 2001-2006,2011,2015-2016 Greg Becker.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: main.c 386 2016-01-27 13:25:47Z greg $
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sysexits.h>
#include <getopt.h>
#include <sys/param.h>

#include "main.h"
#include "clp.h"

#define NDEBUG

static char svnid[] = "$Id: main.c 386 2016-01-27 13:25:47Z greg $";

FILE *fp_conf;

char *progname;
//int verbosity;
int dryrun;

int myint;
u_int myuint;
long mylong;

char *dst_path;

clp_posparam_cb_t posparamv_default_after;
clp_posparam_cb_t posparamv_list_after;

/* Default positional parameters.
 */
clp_posparam_t posparamv_default[] = {
#if 1
    {
        .name = "[leftmost]",
        .help = "optional left-most parameter",
        .after = posparamv_default_after,
    },

    {
        .name = "[left]",
        .help = "optional left parameter",
        .after = posparamv_default_after,
    },

#if 1
    {
        .name = "src...",
        .help = "one or more source files",
        .after = posparamv_default_after,
    },
#else
    {
        .name = "dst",
        .help = "destination directory",
        .convert = clp_convert_string, .result = &dst_path,
        .after = posparamv_default_after,
    },

    {
        .name = "[right]",
        .help = "optional right parameter",
        .after = posparamv_default_after,
    },

#endif

#else

    {
        .name = "src...",
        .help = "one or more source files",
        .after = posparamv_default_after,
    },

    {
        .name = "dst",
        .help = "destination directory",
        .convert = clp_convert_string, .result = &dst_path,
        .after = posparamv_default_after,
    },

#endif
    { .name = NULL }
};

/* Positional parameters for the list option (-l, --list)
 */
clp_posparam_t posparamv_list[] = {
    {
        .name = "[file...]",
        .help = "zero or more files",
        .after = posparamv_list_after,
    },

    { .name = NULL }
};

clp_option_t optionv[] = {
    CLP_OPTION_VERBOSE(&verbosity),
    CLP_OPTION_DRYRUN(&dryrun),
    CLP_OPTION_VERSION(svnid),
    CLP_OPTION_CONF(&fp_conf),
    CLP_OPTION_HELP,

    { // Example of a simple integer argument option
        .optopt = 'i', .argname = "int",
        .help = "specify an int",
        .convert = clp_convert_int, .result = &myint, .cvtarg = (void *)10,
    },

#if 0
    // Example of a bunch of mutually exclusive boolean options.
    { .optopt = '0', .excludes = "1234", .help = "specify a flag", },
    { .optopt = '1', .excludes = "0234", .help = "specify a flag", },
    { .optopt = '2', .excludes = "0134", .help = "specify a flag", },
    { .optopt = '3', .excludes = "0124", .help = "specify a flag", },
    { .optopt = '4', .excludes = "0123", .help = "specify a flag", },

    { .optopt = 'x', .excludes = "y", .help = "specify a flag", },
    { .optopt = 'y', .excludes = "xz", .help = "specify a flag", },
    { .optopt = 'z', .excludes = "y", .help = "specify a flag", },
#endif


    { // Example of a boolean option that triggers non-default positional parameters
        .optopt = 'l', .longopt = "list",
        .help = "list...",
        .paramv = posparamv_list,
    },

    { // Example of a secret boolean option that doesn't show up in help output.
        .optopt = 's', .longopt = "secret",
    },

    { .optopt = 0 }
};

void
posparamv_default_after(struct clp_posparam_s *param)
{
    int i;

    if (param->argc == 0) {
        printf("%s: name=%-12s  unfilled\n",
               __func__, param->name);
    }

    for (i = 0; i < param->argc; ++i) {
        printf("%s: name=%-12s  argv[%d]=%s\n",
               __func__, param->name, i, param->argv[i]);
    }
           
}

void
posparamv_list_after(struct clp_posparam_s *param)
{
    int i;

    for (i = 0; i < param->argc; ++i) {
        printf("%s: %s\n", __func__, param->argv[i]);
    }
           
}

int
main(int argc, char **argv)
{
    char errbuf[CLP_ERRBUFSZ];
    int optind;
    int rc;

    progname = strrchr(argv[0], '/');
    progname = (progname ? progname + 1 : argv[0]);

    if (1) {
        rc = clp_parsev(argc, argv, optionv, posparamv_default, errbuf, &optind);
        if (rc) {
            fprintf(stderr, "%s: %s\n", progname, errbuf);
            exit(rc);
        }
    }
    else if (argc > 1) {
        rc = clp_parsel(argv[1], optionv, posparamv_default, errbuf);
        if (rc) {
            fprintf(stderr, "%s: %s\n", progname, errbuf);
            exit(rc);
        }
    }

    /* Call your function here...
     */
    //return func();

    return 0;
}
