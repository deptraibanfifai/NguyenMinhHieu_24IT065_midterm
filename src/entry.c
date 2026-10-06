#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entry.h"

void entry_list_init(entry_list_t *list)
{
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}

void entry_list_free(entry_list_t *list)
{
    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i].name);
        free(list->items[i].path);
    }
    free(list->items);
    entry_list_init(list);
}

/* Make sure there is room for at least one more element. */
static int list_grow(entry_list_t *list)
{
    if (list->count < list->capacity)
        return 0;

    size_t   new_cap = list->capacity ? list->capacity * 2 : 16;
    entry_t *tmp     = realloc(list->items, new_cap * sizeof(entry_t));
    if (tmp == NULL)
        return -1;

    list->items    = tmp;
    list->capacity = new_cap;
    return 0;
}

/* strdup replacement (not in strict C99). */
static char *copy_string(const char *s)
{
    size_t len = strlen(s) + 1;
    char  *p   = malloc(len);
    if (p != NULL)
        memcpy(p, s, len);
    return p;
}

char *join_path(const char *dir, const char *name)
{
    size_t dlen = strlen(dir);
    size_t nlen = strlen(name);
    char  *p    = malloc(dlen + nlen + 2);
    if (p == NULL)
        return NULL;

    memcpy(p, dir, dlen);
    /* avoid "//" when dir already ends with '/' */
    if (dlen > 0 && dir[dlen - 1] != '/')
        p[dlen++] = '/';
    memcpy(p + dlen, name, nlen + 1);
    return p;
}

int entry_list_add(entry_list_t *list, const char *name, const char *path)
{
    struct stat st;

    if (lstat(path, &st) == -1) {
        fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
        return 1;
    }

    if (list_grow(list) == -1)
        return -1;

    entry_t *e = &list->items[list->count];
    e->name = copy_string(name);
    e->path = copy_string(path);
    if (e->name == NULL || e->path == NULL) {
        free(e->name);
        free(e->path);
        return -1;
    }
    e->st      = st;
    e->stat_ok = 1;
    list->count++;
    return 0;
}

int read_directory(const char *dir_path, entry_list_t *list)
{
    DIR *dp = opendir(dir_path);
    if (dp == NULL) {
        fprintf(stderr, "ls: %s: %s\n", dir_path, strerror(errno));
        return -1;
    }

    struct dirent *de;
    errno = 0;
    while ((de = readdir(dp)) != NULL) {
        char *full = join_path(dir_path, de->d_name);
        if (full == NULL) {
            fprintf(stderr, "ls: out of memory\n");
            closedir(dp);
            return -1;
        }
        /* an lstat error on one entry does not stop the whole directory */
        int rc = entry_list_add(list, de->d_name, full);
        free(full);
        if (rc == -1) {
            fprintf(stderr, "ls: out of memory\n");
            closedir(dp);
            return -1;
        }
        errno = 0;
    }
    if (errno != 0)
        fprintf(stderr, "ls: %s: %s\n", dir_path, strerror(errno));

    closedir(dp);
    return 0;
}
