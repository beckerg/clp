# clp
Simple command line processor

clp is (yet another) command line processor that aims to bring consistency and simplicity
to command line parsing.  It is a one-pass parser/processor, in the sense that you can
simply call clp_parsev() to parse and process a given argument vector.  There is no need
to run external programs nor create/initialize objects.

By default clp builds as a static library, but your best best is probably to simply include
clp/lib/clp.[ch] into your project.

TODO:
* Since it leverages getopt_long() it is inherently not thread-safe.
