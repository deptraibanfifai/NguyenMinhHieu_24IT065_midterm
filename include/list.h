#ifndef LIST_H
#define LIST_H

#include "entry.h"
#include "options.h"

/*
 * Remove entries that must not be shown:
 *  default: names starting with '.'
 *  -A:      keep dotfiles except "." and ".."
 *  -a:      keep everything
 */
void filter_hidden(entry_list_t *list, const options_t *opts);

/* List one directory: read, filter, sort, print. Returns 0 on success. */
int list_directory(const char *dir_path, const options_t *opts);

#endif /* LIST_H */
