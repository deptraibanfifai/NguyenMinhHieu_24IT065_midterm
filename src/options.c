#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"

static void usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", progname);
}

int parse_options(int argc, char *argv[], options_t *opts) {
    if (!opts) return -1;

    int c;
    bool nonprint_set = false;

    /* Set default values */
    *opts = (options_t){0};
    opts->time_kind = TIME_MTIME;
    opts->size_kind = SIZE_BLOCK;

    while ((c = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (c) {
            case 'A': opts->almost_all = true; break;
            case 'a': opts->all = true; break;
            case 'c': opts->time_kind = TIME_CTIME; break;
            case 'u': opts->time_kind = TIME_ATIME; break;
            case 'd': opts->dir_as_file = true; opts->recursive = false; break;
            case 'R': if (!opts->dir_as_file) opts->recursive = true; break;
            case 'F': opts->classify = true; break;
            case 'f': opts->no_sort = true; break;
            case 'h': opts->size_kind = SIZE_HUMAN; break;
            case 'k': opts->size_kind = SIZE_KILO; break;
            case 'i': opts->inode = true; break;
            case 'l': opts->long_fmt = true; opts->numeric = false; break;
            case 'n': opts->long_fmt = true; opts->numeric = true; break;
            case 'q': opts->nonprint = NP_QUESTION; nonprint_set = true; break;
            case 'w': opts->nonprint = NP_RAW; nonprint_set = true; break;
            case 'r': opts->reverse = true; break;
            case 'S': opts->sort_size = true; break;
            case 's': opts->show_blocks = true; break;
            case 't': opts->sort_time = true; break;
            default:
                usage(argv[0]);
                return -1;
        }
    }

    /* Handle default behavior for non-printable characters */
    if (!nonprint_set) {
        opts->nonprint = isatty(STDOUT_FILENO) ? NP_QUESTION : NP_RAW;
    }

    /* Root user (uid 0) defaults to -A unless overridden */
    if (geteuid() == 0) {
        opts->almost_all = true;
    }

    return optind;
}
