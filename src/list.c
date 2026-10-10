#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "print.h"
#include "sort.h"

void filter_hidden(entry_list_t *list, const options_t *opts)
{
    if (opts->all)
        return;

    size_t kept = 0;
    for (size_t i = 0; i < list->count; i++) {
        entry_t *e = &list->items[i];
        int keep;

        if (e->name[0] != '.')
            keep = 1;
        else if (opts->almost_all)
            keep = strcmp(e->name, ".") != 0 && strcmp(e->name, "..") != 0;
        else
            keep = 0;

        if (keep) {
            if (kept != i)
                list->items[kept] = *e;   /* compact in place */
            kept++;
        } else {
            free(e->name);                /* free dropped entry */
            free(e->path);
        }
    }
    list->count = kept;
}

int list_directory(const char *dir_path, const options_t *opts)
{
    entry_list_t list;
    entry_list_init(&list);

    if (read_directory(dir_path, &list) != 0) {
        entry_list_free(&list);
        return -1;
    }

    filter_hidden(&list, opts);
    sort_entries(&list, opts);
    print_total(&list, opts);
    if (opts->long_fmt)
        print_long(&list, opts);
    else
        print_short(&list, opts);

    entry_list_free(&list);
    return 0;
}
