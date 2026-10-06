#ifndef ENTRY_H
#define ENTRY_H

#include <sys/stat.h>
#include <stddef.h>
#include "options.h"

/* One item (file/directory) to display */
typedef struct {
    char        *name;      /* display name */
    char        *path;      /* full path used for lstat/opendir */
    struct stat  st;        /* result of lstat */
    int          stat_ok;   /* 1 if lstat succeeded */
} entry_t;

/* Dynamic list of entries */
typedef struct {
    entry_t *items;
    size_t   count;
    size_t   capacity;
} entry_list_t;

void entry_list_init(entry_list_t *list);
void entry_list_free(entry_list_t *list);

/*
 * Create an entry from name and path (calls lstat) and append it.
 * Returns 0 on success, 1 if lstat failed (error printed, not added),
 * -1 on out of memory.
 */
int entry_list_add(entry_list_t *list, const char *name, const char *path);

/*
 * Read all entries of dir_path into list (unfiltered, unsorted).
 * Returns 0 on success, -1 if the directory cannot be read.
 */
int read_directory(const char *dir_path, entry_list_t *list);

/* Build "dir/name" (heap allocated, caller frees). */
char *join_path(const char *dir, const char *name);

#endif /* ENTRY_H */
