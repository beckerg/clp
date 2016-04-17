# clp
Simple command line processor

clp is (yet another) command line processor that aims to bring consistency and simplicity
to command line parsing.  It is a one-pass parser/processor, in the sense that you can
simply call clp_parsev() to parse and process a given argument vector.

You can either link your application with libclp, or simply compile clp.c/clp.h directly
into your application.  There are no external programs to run to generate the parser,
nor are there any functions to call to set things up (e.g., some tools of this ilk
require creating an object for each option).

By default, clp builds as a static library, but your best best is probably to simply
include clp/lib/clp.[ch] into your project.

TODO:
* Since it leverages getopt_long() it is inherently not thread-safe.
