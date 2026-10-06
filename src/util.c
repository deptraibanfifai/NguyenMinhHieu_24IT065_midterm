#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

void format_mode(mode_t m, char *buf)
{
    /* entry type */
    if (S_ISDIR(m))       buf[0] = 'd';
    else if (S_ISLNK(m))  buf[0] = 'l';
    else if (S_ISBLK(m))  buf[0] = 'b';
    else if (S_ISCHR(m))  buf[0] = 'c';
    else if (S_ISSOCK(m)) buf[0] = 's';
    else if (S_ISFIFO(m)) buf[0] = 'p';
#ifdef S_ISWHT
    else if (S_ISWHT(m))  buf[0] = 'w';
#endif
    else                  buf[0] = '-';

    /* owner */
    buf[1] = (m & S_IRUSR) ? 'r' : '-';
    buf[2] = (m & S_IWUSR) ? 'w' : '-';
    if (m & S_ISUID)
        buf[3] = (m & S_IXUSR) ? 's' : 'S';
    else
        buf[3] = (m & S_IXUSR) ? 'x' : '-';

    /* group */
    buf[4] = (m & S_IRGRP) ? 'r' : '-';
    buf[5] = (m & S_IWGRP) ? 'w' : '-';
    if (m & S_ISGID)
        buf[6] = (m & S_IXGRP) ? 's' : 'S';
    else
        buf[6] = (m & S_IXGRP) ? 'x' : '-';

    /* other */
    buf[7] = (m & S_IROTH) ? 'r' : '-';
    buf[8] = (m & S_IWOTH) ? 'w' : '-';
    if (m & S_ISVTX)
        buf[9] = (m & S_IXOTH) ? 't' : 'T';
    else
        buf[9] = (m & S_IXOTH) ? 'x' : '-';

    buf[10] = '\0';
}

void format_time(time_t t, char *buf, size_t buflen)
{
    struct tm tmv;
    time_t    now = time(NULL);
    /* about 6 months */
    const time_t six_months = 182L * 24 * 60 * 60;

    if (localtime_r(&t, &tmv) == NULL) {
        snprintf(buf, buflen, "?");
        return;
    }
    if (t > now + 60 || now - t > six_months)
        strftime(buf, buflen, "%b %e  %Y", &tmv);   /* old/future: year */
    else
        strftime(buf, buflen, "%b %e %H:%M", &tmv);
}
