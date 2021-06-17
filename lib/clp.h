/*
 * Copyright (c) 2015-2016,2021 Greg Becker.  All rights reserved.
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
 * $Id: clp.h 384 2016-01-25 11:48:25Z greg $
 */
#ifndef CLP_H
#define CLP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sysexits.h>
#include <sys/types.h>

#define CLP_OPTION_END      { .optopt = 0 }
#define CLP_POSPARAM_END    { .name = NULL }

/* List of conversion routines provided by clp.  The name from the "_xtype"
 * column can be given as the fifth argument to CLP_OPTION(), or appended
 * to "clp_cvt_" and either assigned to .cvtfunc or given as the fifth
 * argument to CLP_OPTION_TMPL().
 *
 * _xtype       .cvtdst         .cvtflags       .cvtparms
 *
 * bool         bool *
 * char         char *
 * u_char       u_char *
 * short        short *
 * u_short      u_short *
 * int          int *
 * u_int        u_int *
 * long         long *
 * u_long       u_long *
 * intXX_t      intXX_t *
 * uintXX_t     uintXX_t *
 *
 * intmax_t     intmax_t *
 * uintmax_t    uintmax_t *
 *
 * intptr_t     intptr_t *
 * uintptr_t    uintptr_t *
 *
 * size_t       size_t *
 * time_t       time_t *
 *
 * The above conversion functions accept a clp_cvtparms_t pointer
 * in the cvtparms parameter in order to process a list of integers.
 *
 * string       char *
 * fp           FILE **         -               fopen() mode arg
 * fd           int *           open() flags    -
 */

/* _xoptopt    option letter for getopt (e.g., 'x')
 * _xtype      option type (e.g., int, long, ...)
 * _xvarname   name of variable in which to store converted optarg (e.g., xarg)
 * _xexcludes  list of mutually exclusive options (e.g., "yz")
 * _xparamv    ptr to posparam vector unique to this option
 * _xhelp      help string for -h (e.g. "specify x coordinate")
 *
 * CLP_OPTION() is used to define simple options for which _xvarname
 * is statically defined.  The type of _xvarname must match _xtype.
 */
#define CLP_OPTION(_xoptopt, _xtype, _xvarname, _xexcludes, _xparamv, _xhelp) \
    {                                                                   \
        .optopt = (_xoptopt),                                           \
        .argname = #_xvarname,                                          \
        .excludes = (_xexcludes),                                       \
        .getfunc = clp_get_ ## _xtype,                                  \
        .cvtfunc = clp_cvt_ ## _xtype,                                  \
        .cvtdst = &(_xvarname),                                         \
        .paramv = (_xparamv),                                           \
        .help = (_xhelp),                                               \
    }

/* Like CLP_OPTION() but required for thread-local or dynamically created
 * option variables (i.e., _xvarname is not statically defined).
 * Converted optarg is only available via the last argument to clp_given()
 * (e.g., given '__thread int x' call clp_given('x', optionv, &x) to
 * retrieve the converted optarg if -x was given on the command line).
 */
#define CLP_OPTION_TLS(_xoptopt, _xtype, _xvarname, _xexcludes, _xparamv, _xhelp) \
    {                                                                   \
        .optopt = (_xoptopt),                                           \
        .argname = #_xvarname,                                          \
        .excludes = (_xexcludes),                                       \
        .getfunc = clp_get_ ## _xtype,                                  \
        .cvtfunc = clp_cvt_ ## _xtype,                                  \
        .paramv = (_xparamv),                                           \
        .help = (_xhelp),                                               \
    }

/* Use the HELP, VERSION, VERBOSITY, DRYRUN, and CONF templates to ensure
 * a consistent look-and-free across all tools built with clp.
 */
#define CLP_OPTION_HELP()                                               \
    {                                                                   \
        .optopt = 'h', .excludes = "*", .longopt = "help",              \
        .after = clp_help, .paramv = clp_posparam_none,                 \
        .help = "print this help list",                                 \
    }

#define CLP_OPTION_VERSION(_xversion)                                   \
    {                                                                   \
        .optopt = 'V', .excludes = "*", .longopt = "version",           \
        .after = clp_version, .paramv = clp_posparam_none,              \
        .cvtdst = &(_xversion), .help = "print version",                \
    }

#define CLP_OPTION_VERBOSITY(_xverbosity)                               \
    CLP_OPTION('v', incr, _xverbosity, NULL, NULL, "increase verbosity")

#define CLP_OPTION_DRYRUN(_xdryrun)                                     \
    CLP_OPTION('n', incr, _xdryrun, NULL, NULL, "dry run")

#define CLP_OPTION_CONF(_xconf)                                         \
    CLP_OPTION('C', fp, _xconf, NULL, NULL, "specify a config file")

/* Use CLP_OPTION_TMPL() to generate options with custom optarg converters
 * and/or to specify callbacks to be called before/after option processing.
 */
#define CLP_OPTION_TMPL(_xoptopt, _xvarname, _xexcludes, _xlongopt,     \
                        _xcvtfunc, _xcvtflags, _xcvtparms, _xcvtdst,    \
                        _xbefore, _xafter, _xparamv, _xhelp)            \
    {                                                                   \
        .optopt = (_xoptopt),                                           \
        .argname = (_xvarname),                                         \
        .excludes = (_xexcludes),                                       \
        .longopt = (_xlongopt),                                         \
        .cvtfunc = (_xcvtfunc),                                         \
        .cvtflags = (_xcvtflags),                                       \
        .cvtparms = (_xcvtparms),                                       \
        .cvtdst = (_xcvtdst),                                           \
        .before = (_xbefore),                                           \
        .after = (_xafter),                                             \
        .paramv = (_xparamv),                                           \
        .help = (_xhelp),                                               \
    }


#define CLP_POSPARAM(_xppname, _xpphelp, _xppbefore, _xppafter) \
    {                                                           \
        .name = (_xppname),                                     \
        .help = (_xpphelp),                                     \
        .before = (_xppbefore),                                 \
        .after = (_xppafter),                                   \
    }

struct clp;
struct clp_option;
struct clp_posparam;

typedef int clp_cvt_cb(const char *str, int flags, void *parms, void *dst);
typedef void clp_get_cb(struct clp_option *option, void *dst);

typedef void clp_option_cb(struct clp_option *option);

typedef void clp_posparam_cb(struct clp_posparam *param);

struct clp_posparam {
    const char          *name;          // Name shown by help for the parameter
    const char          *help;          // One line that descibes this parameter
    clp_cvt_cb          *cvtfunc;       // Called for each positional argument
    int                  cvtflags;      // Arg 2 to cvtfunc()
    void                *cvtparms;      // Arg 3 to cvtfunc()
    void                *cvtdst;        // Where cvtfunc() stores its output
    clp_posparam_cb     *before;        // Called before positional argument distribution
    clp_posparam_cb     *after;         // Called after positional argument distribution
    void                *priv;          // Free for use by caller of clp_parse()

    /* The following fields are used by the option parser, whereas the above
     * fields are supplied by the user.
     */
    struct clp_posparam *next;          // posparam list linkage
    struct clp          *clp;
    int                  posmin;        // Min number of positional parameters
    int                  posmax;        // Max number of positional parameters
    int                  argc;          // Number of arguments assigned to this parameter
    char               **argv;          // Ptr to arguments assigned to this parameter
    unsigned char        cvtdstbuf[16] __aligned(16);
};

struct clp_option {
    const int            optopt;        // Option letter for getopt optstring
    const char          *argname;       // Name of option argument (if any)
    const char          *excludes;      // List of options excluded by optopt
    const char          *longopt;       // Long option name for getopt
    const char          *help;          // One line that describes this option
    clp_get_cb          *getfunc;       // Function to retrieve converted argument
    clp_cvt_cb          *cvtfunc;       // Function to convert optarg
    int                  cvtflags;      // Arg 2 to cvtfunc()
    void                *cvtparms;      // Arg 3 to cvtfunc()
    void                *cvtdst;        // Where cvtfunc() stores its result
    clp_option_cb       *before;        // Called before getopt processing
    clp_option_cb       *after;         // Called after getopt processing
    struct clp_posparam *paramv;        // Option specific positional parameters
    void                *priv;          // Free for use by caller of clp_parse()

    /* The following fields are used by the option parser, whereas the above
     * fields are supplied by the user.
     */
    struct clp_option   *next;          // option list linkage
    struct clp          *clp;           // Not valid when parser returns
    const char          *optarg;        // optarg from getopt()
    int                  given;         // Count of times this option was given
    int                  longidx;       // Index into cli->longopts[]
    unsigned char        cvtdstbuf[16] __aligned(16);
};

struct clp {
    const char          *basename;      // From argv[0] of clp_parsev()
    struct clp_option   *optionv;       // Argument from clp_parsev()
    struct clp_posparam *paramv;        // Argument from clp_parsev()
    int                  opthelp;       // The option tied to opt_help()
    int                  optionc;       // Count of elements in optionv[]
    char                *optstring;     // The optstring for getopt
    struct option       *longopts;      // Table of long options for getopt_long()
    char                *errbuf;
    size_t               errbufsz;
    struct clp_posparam *params;        // posparam list head
};

struct clp_suftab {
    const char *list;
    double mult[];
};

/* Declare a type-specific vector.
 */
#define CLP_VECTOR_DECL(_xname, _xtype, _xsize)                         \
    struct _xname {                                                     \
        unsigned int     len;                                           \
        unsigned int     size;                                          \
        const char      *delim;                                         \
        void            *priv;                                          \
        _xtype           data[(_xsize)];                                \
    }

/* Declare, define, and initialize a vector.
 */
#define CLP_VECTOR(_xname, _xtype, _xsize, _xdelim)                    \
    CLP_VECTOR_DECL(_xname, _xtype, _xsize) _xname = {                 \
        .size = (_xsize),                                              \
        .delim = (_xdelim),                                            \
    }

typedef CLP_VECTOR_DECL(clp_vector, char, 0) clp_vector_t;

/* This template produces type-specific functions to convert a string
 * of one or more delimited numbers to a single/vector of integers.
 *
 * Each string to be converted may end in a single character suffix
 * from suftab which modifies the result.
 *
 * Note that we use strtold() to parse each number in order to allow
 * the caller maximum flexibility when specifying number formats.
 * There is the possibility for loss of precision if long double
 * on the target platform doesn't have at least as many bits in the
 * significand as the widest integer type for which this function
 * may be called.
 */
#define CLP_CVT_TMPL(_xsuffix, _xtype, _xmin, _xmax, _xsuftab)          \
__attribute__((__visibility__("hidden")))                               \
int                                                                     \
clp_cvt_ ## _xsuffix(const char *optarg, int flags, void *parms, void *dst) \
{                                                                       \
    const struct clp_suftab *suftab = &(_xsuftab);                      \
    CLP_VECTOR(vectorbuf, _xtype, 1, "");                               \
    clp_vector_t *vector;                                               \
    char *str, *strbase;                                                \
    _xtype *result;                                                     \
    bool domainchk;                                                     \
    int ndomain, nrange;                                                \
    int n;                                                              \
                                                                        \
    if (!optarg || !dst) {                                              \
        errno = EINVAL;                                                 \
        return EX_DATAERR;                                              \
    }                                                                   \
                                                                        \
    vector = (void *)&vectorbuf;                                        \
    if (parms) {                                                        \
        vector = parms;                                                 \
    }                                                                   \
                                                                        \
    /* Only call strdup if there are delimiters in optarg.              \
     */                                                                 \
    str = (char *)optarg;                                               \
    strbase = strpbrk(str, vector->delim);                              \
    if (strbase) {                                                      \
        strbase = strdup(optarg);                                       \
        if (!strbase) {                                                 \
            errno = ENOMEM;                                             \
            return EX_DATAERR;                                          \
        }                                                               \
        str = strbase;                                                  \
    }                                                                   \
                                                                        \
    domainchk = (_xmin) < (_xmax);                                      \
    result = dst;                                                       \
    ndomain = 0;                                                        \
    nrange = 0;                                                         \
    errno = 0;                                                          \
                                                                        \
    for (n = 0; n < vector->size && str; ++n, ++result) {               \
        char *tok, *end;                                                \
        long double val;                                                \
                                                                        \
        if (strbase) {                                                  \
            tok = strsep(&str, vector->delim);                          \
            if (tok && *tok == '\000') {                                \
                *result = 0;                                            \
                continue;                                               \
            }                                                           \
        } else {                                                        \
            tok = str;                                                  \
            str = NULL;                                                 \
        }                                                               \
                                                                        \
        errno = 0;                                                      \
        val = strtold(tok, &end);                                       \
                                                                        \
        if (errno) {                                                    \
            if (errno != ERANGE) {                                      \
                *result = (_xtype)val;                                  \
                break;                                                  \
            }                                                           \
            ++nrange;                                                   \
        }                                                               \
                                                                        \
        if (end == tok) {                                               \
            errno = EINVAL;                                             \
            *result = 0;                                                \
            break;                                                      \
        }                                                               \
                                                                        \
        if (*end) {                                                     \
            const char *pc;                                             \
                                                                        \
            pc = strchr(suftab->list, *end);                            \
            if (!pc) {                                                  \
                errno = EINVAL;                                         \
                *result = 0;                                            \
                break;                                                  \
            }                                                           \
                                                                        \
            val *= *(suftab->mult + (pc - suftab->list));               \
        }                                                               \
                                                                        \
        if (isinf(val) || isnan(val))                                   \
            ;                                                           \
        else if (domainchk && (val < (_xmin) || val > (_xmax))) {       \
            val = (val < (_xmin)) ? (_xmin) : (_xmax);                  \
            ++ndomain;                                                  \
        }                                                               \
                                                                        \
        *result = (_xtype)val;                                          \
    }                                                                   \
                                                                        \
    vector->len = n;                                                    \
    if (str && vector->len >= vector->size) {                           \
        errno = E2BIG;                                                  \
    }                                                                   \
                                                                        \
    if (ndomain > 0 && !errno) {                                        \
        errno = EDOM;                                                   \
    }                                                                   \
    else if (nrange > 0 && !errno) {                                    \
        errno = ERANGE;                                                 \
    }                                                                   \
                                                                        \
    if (strbase) {                                                      \
        int save = errno;                                               \
                                                                        \
        free(strbase);                                                  \
        errno = save;                                                   \
    }                                                                   \
                                                                        \
    return errno ? EX_DATAERR : 0;                                      \
}                                                                       \
                                                                        \
__attribute__((__visibility__("hidden")))                               \
void                                                                    \
clp_get_ ## _xsuffix(struct clp_option *option, void *dst)              \
{                                                                       \
    _xtype *src = (_xtype *)option->cvtdst;                             \
                                                                        \
    if (!src)                                                           \
        src = (_xtype *)option->cvtdstbuf;                              \
                                                                        \
    *(_xtype *)dst = *src;                                              \
}


#define CLP_GET_TMPL(_xsuffix, _xtype)                                  \
__attribute__((__visibility__("hidden")))                               \
void                                                                    \
clp_get_ ## _xsuffix(struct clp_option *option, void *dst)              \
{                                                                       \
    _xtype *src = (_xtype *)option->cvtdst;                             \
                                                                        \
    if (!src)                                                           \
        src = (_xtype *)option->cvtdstbuf;                              \
                                                                        \
    *(_xtype *)dst = *src;                                              \
}


extern struct clp_posparam clp_posparam_none[];

extern struct clp_option *clp_find(int optopt, struct clp_option *optionv);
extern struct clp_option *clp_given(int optopt, struct clp_option *optionv, void *dst);

extern struct clp_suftab clp_suftab_si;
extern struct clp_suftab clp_suftab_iec;
extern struct clp_suftab clp_suftab_combo;
extern struct clp_suftab clp_suftab_none;
extern struct clp_suftab clp_suftab_time;

extern clp_cvt_cb clp_cvt_fp;
extern clp_cvt_cb clp_cvt_fd;
extern clp_cvt_cb clp_cvt_bool;
extern clp_cvt_cb clp_cvt_incr;
extern clp_cvt_cb clp_cvt_string;

extern clp_cvt_cb clp_cvt_char, clp_cvt_u_char;
extern clp_cvt_cb clp_cvt_short, clp_cvt_u_short;
extern clp_cvt_cb clp_cvt_int, clp_cvt_u_int;
extern clp_cvt_cb clp_cvt_long, clp_cvt_u_long;
extern clp_cvt_cb clp_cvt_float, clp_cvt_double;

extern clp_cvt_cb clp_cvt_int8, clp_cvt_int8_t;
extern clp_cvt_cb clp_cvt_uint8, clp_cvt_uint8_t;

extern clp_cvt_cb clp_cvt_int16, clp_cvt_int16_t;
extern clp_cvt_cb clp_cvt_uint16, clp_cvt_uint16_t;

extern clp_cvt_cb clp_cvt_int32, clp_cvt_int32_t;
extern clp_cvt_cb clp_cvt_uint32, clp_cvt_uint32_t;

extern clp_cvt_cb clp_cvt_int64, clp_cvt_int64_t;
extern clp_cvt_cb clp_cvt_uint64, clp_cvt_uint64_t;

extern clp_cvt_cb clp_cvt_intmax_t, clp_cvt_uintmax_t;
extern clp_cvt_cb clp_cvt_intptr_t, clp_cvt_uintptr_t;

extern clp_cvt_cb clp_cvt_size_t;
extern clp_cvt_cb clp_cvt_time_t;

extern clp_get_cb clp_get_fp;
extern clp_get_cb clp_get_fd;
extern clp_get_cb clp_get_bool;
extern clp_get_cb clp_get_incr;
extern clp_get_cb clp_get_string;

extern clp_get_cb clp_get_char, clp_get_u_char;
extern clp_get_cb clp_get_short, clp_get_u_short;
extern clp_get_cb clp_get_int, clp_get_u_int;
extern clp_get_cb clp_get_long, clp_get_u_long;
extern clp_get_cb clp_get_float, clp_get_double;

extern clp_get_cb clp_get_int8, clp_get_int8_t;
extern clp_get_cb clp_get_uint8, clp_get_uint8_t;

extern clp_get_cb clp_get_int16, clp_get_int16_t;
extern clp_get_cb clp_get_uint16, clp_get_uint16_t;

extern clp_get_cb clp_get_int32, clp_get_int32_t;
extern clp_get_cb clp_get_uint32, clp_get_uint32_t;

extern clp_get_cb clp_get_int64, clp_get_int64_t;
extern clp_get_cb clp_get_uint64, clp_get_uint64_t;

extern clp_get_cb clp_get_intmax_t, clp_get_uintmax_t;
extern clp_get_cb clp_get_intptr_t, clp_get_uintptr_t;

extern clp_get_cb clp_get_size_t;
extern clp_get_cb clp_get_time_t;


extern clp_option_cb clp_help;
extern clp_option_cb clp_version;

extern int clp_breakargs(const char *src, const char *delim,
                         char *errbuf, size_t errbufsz,
                         int *argcp, char ***argvp);

extern int clp_parsev(int argc, char **argv,
                      struct clp_option *optionv,
                      struct clp_posparam *paramv,
                      char *errbuf, size_t errbufsz);

extern int clp_parsel(const char *line, const char *delim,
                      struct clp_option *optionv,
                      struct clp_posparam *paramv,
                      char *errbuf, size_t errbufsz);

extern int optind;

#endif /* CLP_H */
