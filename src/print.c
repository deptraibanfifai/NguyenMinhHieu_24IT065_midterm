#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef __linux__
#include <sys/sysmacros.h>
#endif
#include <unistd.h>
#include "print.h"
#include "util.h"

void print_name(const char *name, const options_t *opts)
{
    for (const char *p = name; *p != '\0'; p++) {
        unsigned char c = (unsigned char)*p;
        if (opts->nonprint == NP_QUESTION && !isprint(c))
            putchar('?');
        else
            putchar(c);
    }
}

/* Value of the -s column for one entry (handles -h, -k, BLOCKSIZE). */
static void format_blocks(const entry_t *e, const options_t *opts,
                          char *buf, size_t buflen)
{
    unsigned long long b512 = (unsigned long long)e->st.st_blocks;

    if (opts->size_kind == SIZE_HUMAN)
        humanize_bytes(b512 * 512ULL, buf, buflen);
    else
        snprintf(buf, buflen, "%llu",
                 blocks_to_units(b512, get_block_size(opts)));
}

void print_total(const entry_list_t *list, const options_t *opts)
{
    /* -l always prints total; -s alone only on a terminal */
    if (!(opts->long_fmt || (opts->show_blocks && isatty(STDOUT_FILENO))))
        return;

    unsigned long long sum512 = 0;
    for (size_t i = 0; i < list->count; i++)
        sum512 += (unsigned long long)list->items[i].st.st_blocks;

    if (opts->size_kind == SIZE_HUMAN) {
        char buf[32];
        humanize_bytes(sum512 * 512ULL, buf, sizeof(buf));
        printf("total %s\n", buf);
    } else {
        printf("total %llu\n",
               blocks_to_units(sum512, get_block_size(opts)));
    }
}

void print_short(const entry_list_t *list, const options_t *opts)
{
    char blocks[32];
    char inode[32];
    size_t bw = 0;
    size_t iw = 0;

    for (size_t i = 0; i < list->count; i++) {
        const entry_t *e = &list->items[i];
        format_blocks(e, opts, blocks, sizeof(blocks));
        snprintf(inode, sizeof(inode), "%llu",
                 (unsigned long long)e->st.st_ino);
        if (strlen(blocks) > bw)
            bw = strlen(blocks);
        if (strlen(inode) > iw)
            iw = strlen(inode);
    }
    for (size_t i = 0; i < list->count; i++) {
        const entry_t *e = &list->items[i];
        if (opts->inode)
            printf("%*llu ", (int)iw,
                   (unsigned long long)e->st.st_ino);
        if (opts->show_blocks) {
            format_blocks(e, opts, blocks, sizeof(blocks));
            printf("%*s ", (int)bw, blocks);
        }
        print_name(e->name, opts);
        if (opts->classify) {
            char c = classify_char(&e->st);
            if (c != '\0')
                putchar(c);
        }
        putchar('\n');
    }
}

/* ---------- long format ---------- */

/* One row of pre-formatted text columns. */
typedef struct {
    char ino[24];
    char blk[32];
    char mode[12];
    char nlink[24];
    char owner[64];
    char group[64];
    char size[40];
    char date[40];
} row_t;

/* Resolve uid/gid to a name, or a number if unknown or -n. */
static void format_owner(uid_t uid, const options_t *opts,
                         char *buf, size_t len)
{
    struct passwd *pw = opts->numeric ? NULL : getpwuid(uid);
    if (pw != NULL)
        snprintf(buf, len, "%s", pw->pw_name);
    else
        snprintf(buf, len, "%llu", (unsigned long long)uid);
}

static void format_group(gid_t gid, const options_t *opts,
                         char *buf, size_t len)
{
    struct group *gr = opts->numeric ? NULL : getgrgid(gid);
    if (gr != NULL)
        snprintf(buf, len, "%s", gr->gr_name);
    else
        snprintf(buf, len, "%llu", (unsigned long long)gid);
}

/* Pick the timestamp selected by -c / -u (default mtime). */
static time_t pick_time(const struct stat *st, time_kind_t kind)
{
    switch (kind) {
    case TIME_CTIME: return st->st_ctime;
    case TIME_ATIME: return st->st_atime;
    default:         return st->st_mtime;
    }
}

/* Fill all text columns for one entry. */
static void fill_row(row_t *r, const entry_t *e, const options_t *opts)
{
    const struct stat *st = &e->st;

    snprintf(r->ino, sizeof(r->ino), "%llu",
             (unsigned long long)st->st_ino);
    format_blocks(e, opts, r->blk, sizeof(r->blk));
    format_mode(st->st_mode, r->mode);
    snprintf(r->nlink, sizeof(r->nlink), "%llu",
             (unsigned long long)st->st_nlink);
    format_owner(st->st_uid, opts, r->owner, sizeof(r->owner));
    format_group(st->st_gid, opts, r->group, sizeof(r->group));

    if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode))
        snprintf(r->size, sizeof(r->size), "%lu, %lu",
                 (unsigned long)major(st->st_rdev),
                 (unsigned long)minor(st->st_rdev));
    else if (opts->size_kind == SIZE_HUMAN)
        humanize_bytes((unsigned long long)st->st_size,
                       r->size, sizeof(r->size));
    else
        snprintf(r->size, sizeof(r->size), "%lld",
                 (long long)st->st_size);

    format_time(pick_time(st, opts->time_kind), r->date, sizeof(r->date));
}

/* Track the widest value seen for a column. */
static void widen(size_t *w, const char *s)
{
    size_t n = strlen(s);
    if (n > *w)
        *w = n;
}

void print_long(const entry_list_t *list, const options_t *opts)
{
    if (list->count == 0)
        return;

    row_t *rows = calloc(list->count, sizeof(row_t));
    if (rows == NULL) {
        fprintf(stderr, "ls: out of memory\n");
        exit(EXIT_FAILURE);
    }

    /* pass 1: format every row and find column widths */
    size_t w_ino = 0, w_blk = 0, w_nl = 0, w_own = 0, w_grp = 0, w_sz = 0;
    for (size_t i = 0; i < list->count; i++) {
        fill_row(&rows[i], &list->items[i], opts);
        widen(&w_ino, rows[i].ino);
        widen(&w_blk, rows[i].blk);
        widen(&w_nl,  rows[i].nlink);
        widen(&w_own, rows[i].owner);
        widen(&w_grp, rows[i].group);
        widen(&w_sz,  rows[i].size);
    }

    /* pass 2: print */
    for (size_t i = 0; i < list->count; i++) {
        const entry_t *e = &list->items[i];
        const row_t   *r = &rows[i];

        if (opts->inode)
            printf("%*s ", (int)w_ino, r->ino);
        if (opts->show_blocks)
            printf("%*s ", (int)w_blk, r->blk);

        printf("%s  %*s %-*s  %-*s  %*s %s ",
               r->mode,
               (int)w_nl,  r->nlink,
               (int)w_own, r->owner,
               (int)w_grp, r->group,
               (int)w_sz,  r->size,
               r->date);

        print_name(e->name, opts);

        if (opts->classify) {
            char c = classify_char(&e->st);
            if (c != '\0')
                putchar(c);
        }

        /* symbolic link: show " -> target" */
        if (S_ISLNK(e->st.st_mode)) {
            char    target[4096];
            ssize_t n = readlink(e->path, target, sizeof(target) - 1);
            if (n >= 0) {
                target[n] = '\0';
                printf(" -> ");
                print_name(target, opts);
            }
        }
        putchar('\n');
    }

    free(rows);
}
