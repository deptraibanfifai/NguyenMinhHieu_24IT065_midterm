#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sort.h"

static const options_t *g_opts;

/* Preserve sub-second precision when timestamps share a second. */
static struct timespec selected_time(const entry_t *e)
{
    switch (g_opts->time_kind) {
    case TIME_CTIME:
        return e->st.st_ctim;
    case TIME_ATIME:
        return e->st.st_atim;
    default:
        return e->st.st_mtim;
    }
}

static int compare_entries(const void *pa, const void *pb)
{
    const entry_t *a = pa;
    const entry_t *b = pb;
    int result = 0;

    if (g_opts->sort_size) {
        if (a->st.st_size != b->st.st_size)
            result = a->st.st_size < b->st.st_size ? 1 : -1;
    } else if (g_opts->sort_time) {
        struct timespec ta = selected_time(a);
        struct timespec tb = selected_time(b);

        if (ta.tv_sec != tb.tv_sec)
            result = ta.tv_sec < tb.tv_sec ? 1 : -1;
        else if (ta.tv_nsec != tb.tv_nsec)
            result = ta.tv_nsec < tb.tv_nsec ? 1 : -1;
    }

    if (result == 0)
        result = strcmp(a->name, b->name);

    return g_opts->reverse ? -result : result;
}

void sort_entries(entry_list_t *list, const options_t *opts)
{
    if (opts->no_sort || list->count < 2)
        return;

    g_opts = opts;
    qsort(list->items, list->count, sizeof(entry_t), compare_entries);
}
