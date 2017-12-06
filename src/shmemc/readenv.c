/* For license: see LICENSE file at top-level */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "state.h"
#include "shmemu.h"

#include <stdio.h>
#include <stdlib.h>             /* getenv */
#include <string.h>
#include <strings.h>

/*
 * internal helpers
 */

inline static char *
shmemc_getenv(char *name)
{
    return getenv(name);
}

inline static int
option_enabled_test(char *str)
{
    int ret = 0;

    if ((strncasecmp(str, "y", 1) == 0) ||
        (strncasecmp(str, "on", 2) == 0) ||
        (atoi(str) > 0)) {
        ret = 1;
    }

    return ret;
}

/*
 * read & save all our environment variables
 */

#define CHECK_ENV(_e, _name)                        \
    do {                                            \
        (_e) = shmemc_getenv("SHMEM_" #_name);      \
        if ((_e) == NULL) {                         \
            (_e) = shmemc_getenv("SMA_" #_name);    \
        }                                           \
    } while (0)

void
read_environment(void)
{
    char *e;

    /*
     * defined in spec
     */

    proc.env.print_version = 0;
    proc.env.print_info = 0;
    proc.env.def_heap_size = 4 * SHMEMU_MB;
    proc.env.debug = 0;

    CHECK_ENV(e, VERSION);
    if (e != NULL) {
        proc.env.print_version = option_enabled_test(e);
    }
    CHECK_ENV(e, INFO);
    if (e != NULL) {
        proc.env.print_info = option_enabled_test(e);
    }
    CHECK_ENV(e, SYMMETRIC_SIZE);
    if (e != NULL) {
        const int r = shmemu_parse_size(e, &proc.env.def_heap_size);

        if (r != 0) {
            shmemu_fatal("Couldn't work out requested heap size \"%s\"", e);
        }
    }
    CHECK_ENV(e, DEBUG);
    if (e != NULL) {
        proc.env.debug = option_enabled_test(e);
    }

    /*
     * this implementation also has...
     */

    proc.env.debug_file = NULL;

    CHECK_ENV(e, DEBUG_FILE);
    if (e != NULL) {
        proc.env.debug_file = strdup(e); /* free at end */
    }
}

#undef CHECK_ENV

inline static char *
humanize(int v)
{
    return (v == 0) ? "no" : "yes";
}

static const int var_width = 20;
static const int val_width = 12;

void
shmemc_print_env_vars(FILE *stream)
{
    fprintf(stream, "Environment Variable Information\n");
    fprintf(stream, "\n");
    fprintf(stream, "%s\n\n",
            "From specification:");
    fprintf(stream, "%-*s (= %-*s) %s\n",
            var_width,
            "SHMEM_VERSION",
            val_width,
            humanize(proc.env.print_version),
            "print library version at start-up");
    fprintf(stream, "%-*s (= %-*s) %s\n",
            var_width,
            "SHMEM_INFO",
            val_width,
            humanize(proc.env.print_info),
            "print this information");
    fprintf(stream, "%-*s (= %-*lu) %s\n",
            var_width,
            "SHMEM_SYMMETRIC_SIZE",
            val_width,
            proc.env.def_heap_size,
            "set the size of the symmetric heap");
    fprintf(stream, "%-*s (= %-*s) %s\n",
            var_width,
            "SHMEM_DEBUG",
            val_width,
            humanize(proc.env.debug),
            "enable run debugging (if configured)");

    fprintf(stream, "\n");
    fprintf(stream, "%s\n\n",
            "Specific to this implementation:");
    fprintf(stream, "%-*s (= %-*s) %s\n",
            var_width,
            "SHMEM_DEBUG_FILE",
            val_width,
            (proc.env.debug_file != NULL) ? proc.env.debug_file : "none",
            "file to receive debugging information");

    fprintf(stream, "\n\n");

    fflush(stream);
}