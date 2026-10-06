#ifndef PRINT_H
#define PRINT_H

#include "entry.h"
#include "options.h"

/* Print a name; non-printable chars become '?' when -q is active. */
void print_name(const char *name, const options_t *opts);

/* Print "total N": with -l always, with -s only on a terminal. */
void print_total(const entry_list_t *list, const options_t *opts);

/* Print the list in short format, one entry per line. */
void print_short(const entry_list_t *list, const options_t *opts);

/* Print the list in long format (-l / -n), columns aligned. */
void print_long(const entry_list_t *list, const options_t *opts);

#endif /* PRINT_H */
