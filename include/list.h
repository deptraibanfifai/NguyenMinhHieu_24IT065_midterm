#ifndef LIST_H
#define LIST_H

#include "entry.h"
#include "options.h"

/* Filter hidden directory entries according to -a and -A. */
void filter_hidden(entry_list_t *list, const options_t *opts);

/* List the contents of one directory. */
int list_directory(const char *dir_path, const options_t *opts);

/*
 * Handle all operands, with files first and directories second.
 * Returns 0 on success and 1 if any operation fails.
 */
int list_operands(int count, char **paths, const options_t *opts);

#endif
