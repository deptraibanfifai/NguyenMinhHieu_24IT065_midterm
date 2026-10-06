#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <sys/stat.h>
#include "options.h"

/* Block unit in bytes for -s: -k gives 1024, else BLOCKSIZE, else 512. */
unsigned long get_block_size(const options_t *opts);

/* Convert 512-byte blocks to block_size units, rounding up. */
unsigned long long blocks_to_units(unsigned long long blocks512,
                                   unsigned long block_size);

/* Write a human readable size ("1.5K", "23M", "512B") into buf. */
void humanize_bytes(unsigned long long bytes, char *buf, size_t buflen);

/* -F suffix character for st, or '\0' if none. */
char classify_char(const struct stat *st);

#endif /* UTIL_H */
