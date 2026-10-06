#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

unsigned long get_block_size(const options_t *opts)
{
    if (opts->size_kind == SIZE_KILO)
        return 1024UL;

    /* BLOCKSIZE only matters when neither -h nor -k is given */
    const char *env = getenv("BLOCKSIZE");
    if (env != NULL && *env != '\0') {
        char *end;
        unsigned long n = strtoul(env, &end, 10);
        if (end == env)
            n = 512;
        else {
            switch (toupper((unsigned char)*end)) {
            case 'K': n *= 1024UL; break;
            case 'M': n *= 1024UL * 1024UL; break;
            case 'G': n *= 1024UL * 1024UL * 1024UL; break;
            default: break;
            }
        }
        if (n >= 512)
            return n;
    }
    return 512UL;
}

unsigned long long blocks_to_units(unsigned long long blocks512,
                                   unsigned long block_size)
{
    unsigned long long bytes = blocks512 * 512ULL;
    return (bytes + block_size - 1) / block_size;
}

void humanize_bytes(unsigned long long bytes, char *buf, size_t buflen)
{
    static const char suffix[] = { 'B', 'K', 'M', 'G', 'T', 'P' };
    double value = (double)bytes;
    int idx = 0;

    while (value >= 1024.0 && idx < 5) {
        value /= 1024.0;
        idx++;
    }

    if (idx == 0)
        snprintf(buf, buflen, "%lluB", bytes);
    else if (value < 10.0)
        snprintf(buf, buflen, "%.1f%c", value, suffix[idx]);
    else
        snprintf(buf, buflen, "%.0f%c", value, suffix[idx]);
}

char classify_char(const struct stat *st)
{
    mode_t m = st->st_mode;

    if (S_ISDIR(m))  return '/';
    if (S_ISLNK(m))  return '@';
    if (S_ISSOCK(m)) return '=';
    if (S_ISFIFO(m)) return '|';
#ifdef S_ISWHT
    if (S_ISWHT(m))  return '%';
#endif
    if (m & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';
    return '\0';
}
