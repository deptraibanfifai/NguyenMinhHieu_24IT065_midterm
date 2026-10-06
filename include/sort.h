#ifndef SORT_H
#define SORT_H

#include "entry.h"
#include "options.h"

/* Sort list according to -f, -r, -S, -t, -c, -u. */
void sort_entries(entry_list_t *list, const options_t *opts);

#endif /* SORT_H */
