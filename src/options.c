#include <stdio.h>
#include <unistd.h>
#include "options.h"

static void usage(const char *prog)
{
    fprintf(stderr, "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", prog);
}

int parse_options(int argc, char *argv[], options_t *opts)
{
    int  c;
    bool nonprint_set = false;   /* was -q or -w given? */

    /* Defaults */
    *opts = (options_t){0};
    opts->time_kind = TIME_MTIME;
    opts->size_kind = SIZE_BLOCK;

    optind = 1;
    while ((c = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (c) {
        case 'A': opts->almost_all = true; break;
        case 'a': opts->all = true; break;
        case 'c': opts->time_kind = TIME_CTIME; break;
        case 'u': opts->time_kind = TIME_ATIME; break;
        case 'd': opts->dir_as_file = true; opts->recursive = false; break;
        case 'R': opts->recursive = true; opts->dir_as_file = false; break;
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

    /* -q is the default on a terminal, -w otherwise */
    if (!nonprint_set)
        opts->nonprint = isatty(STDOUT_FILENO) ? NP_QUESTION : NP_RAW;

    /* -A is always set for the super-user */
    if (geteuid() == 0)
        opts->almost_all = true;

    return optind;
}
