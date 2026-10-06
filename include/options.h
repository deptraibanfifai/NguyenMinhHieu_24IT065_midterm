#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

/* Which timestamp is used for sorting (-t) and printing (-l) */
typedef enum { TIME_MTIME, TIME_CTIME, TIME_ATIME } time_kind_t;

/* How sizes/blocks are displayed */
typedef enum { SIZE_BLOCK, SIZE_KILO, SIZE_HUMAN } size_kind_t;

/* How non-printable characters in names are shown */
typedef enum { NP_QUESTION, NP_RAW } nonprint_t;

/* All command line options */
typedef struct {
    bool all;          /* -a */
    bool almost_all;   /* -A */
    bool dir_as_file;  /* -d */
    bool classify;     /* -F */
    bool no_sort;      /* -f */
    bool inode;        /* -i */
    bool long_fmt;     /* -l or -n */
    bool numeric;      /* -n */
    bool recursive;    /* -R */
    bool reverse;      /* -r */
    bool sort_size;    /* -S */
    bool show_blocks;  /* -s */
    bool sort_time;    /* -t */
    time_kind_t time_kind;   /* -c / -u */
    size_kind_t size_kind;   /* -h / -k */
    nonprint_t  nonprint;    /* -q / -w */
} options_t;

/*
 * Parse argc/argv and fill opts.
 * Returns index of the first operand in argv, or -1 on invalid option.
 */
int parse_options(int argc, char *argv[], options_t *opts);

#endif /* OPTIONS_H */
