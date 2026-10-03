#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>

/* Loi thi gian dng  sp xp (-t) hoc in (-l) */
typedef enum { TIME_MTIME, TIME_CTIME, TIME_ATIME } time_kind_t;

/* n v hin th kch thc/block */
typedef enum { SIZE_BLOCK, SIZE_KILO, SIZE_HUMAN } size_kind_t;

/* Cch in k t khng in c trong tn tp */
typedef enum { NP_QUESTION, NP_RAW } nonprint_t;

/* Ton b ty chn dng lnh */
typedef struct {
    bool all;          /* -a */
    bool almost_all;   /* -A */
    bool dir_as_file;  /* -d */
    bool classify;     /* -F */
    bool no_sort;      /* -f */
    bool inode;        /* -i */
    bool long_fmt;     /* -l hoc -n */
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
 * Phn tch argc/argv, in vo opts.
 * Tr v ch s ca ton hng u tin trong argv (optind),
 * hoc -1 nu c ty chn khng hp l.
 */
int parse_options(int argc, char *argv[], options_t *opts);

#endif /* OPTIONS_H */
