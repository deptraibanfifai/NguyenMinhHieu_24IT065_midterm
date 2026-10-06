#include <ctype.h>
#include <stdio.h>
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
    if (!opts->show_blocks || !isatty(STDOUT_FILENO))
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

    for (size_t i = 0; i < list->count; i++) {
        const entry_t *e = &list->items[i];

        if (opts->inode)
            printf("%llu ", (unsigned long long)e->st.st_ino);
        if (opts->show_blocks) {
            format_blocks(e, opts, blocks, sizeof(blocks));
            printf("%s ", blocks);
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
