#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sort.h"

/* qsort cannot take extra arguments, so keep opts in a static variable */
static const options_t *g_opts;

/* Timestamp of an entry according to -c / -u (default: mtime). */
static time_t get_time(const entry_t *e, time_kind_t kind)
{
    switch (kind) {
    case TIME_CTIME: return e->st.st_ctime;
    case TIME_ATIME: return e->st.st_atime;
    default:         return e->st.st_mtime;
    }
}

/* Main comparison (without -r). Never use a - b: types are 64-bit. */
static int cmp_entries(const void *pa, const void *pb)
{
    const entry_t *a = pa;
    const entry_t *b = pb;

    if (g_opts->sort_size) {
        if (a->st.st_size != b->st.st_size)
            return (a->st.st_size < b->st.st_size) ? 1 : -1;
    } else if (g_opts->sort_time) {
        time_t ta = get_time(a, g_opts->time_kind);
        time_t tb = get_time(b, g_opts->time_kind);
        if (ta != tb)
            return (ta < tb) ? 1 : -1;      /* newest first */
    }

    return strcmp(a->name, b->name);        /* tie-break / default: name */
}

/* Comparison including -r. */
static int cmp_final(const void *pa, const void *pb)
{
    int r = cmp_entries(pa, pb);
    return g_opts->reverse ? -r : r;
}

void sort_entries(entry_list_t *list, const options_t *opts)
{
    if (opts->no_sort || list->count < 2)
        return;

    g_opts = opts;
    qsort(list->items, list->count, sizeof(entry_t), cmp_final);
}
