#ifndef PRINT_H
#define PRINT_H

#include "entry.h"
#include "options.h"

/* Print a name; non-printable chars become '?' when -q is active. */
void print_name(const char *name, const options_t *opts);

/* Print "total N" (only with -s and when stdout is a terminal). */
void print_total(const entry_list_t *list, const options_t *opts);

/* Print the list in short format, one entry per line. */
void print_short(const entry_list_t *list, const options_t *opts);

#endif /* PRINT_H */
