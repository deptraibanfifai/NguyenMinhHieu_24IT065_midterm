#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <sys/stat.h>
#include <time.h>
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

/* Write mode string like "drwxr-xr-x" (10 chars + '\0') into buf[11]. */
void format_mode(mode_t mode, char *buf);

/* Format a timestamp like "Oct  3 08:55" or "Oct  3  2023" into buf. */
void format_time(time_t t, char *buf, size_t buflen);

#endif /* UTIL_H */
