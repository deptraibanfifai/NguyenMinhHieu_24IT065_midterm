#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "entry.h"

void init_entry_list(entry_list_t *list) {
    if (!list) return;
    list->entries = NULL;
    list->count = 0;
    list->capacity = 0;
    list->total_blocks = 0;
}

void free_entry_list(entry_list_t *list) {
    if (!list) return;
    for (size_t i = 0; i < list->count; i++) {
        free(list->entries[i].name);
        free(list->entries[i].path);
    }
    free(list->entries);
    init_entry_list(list);
}

void add_entry(entry_list_t *list, const char *path, const char *name, const options_t *opts) {
    if (!list || !name) return;

    if (list->count >= list->capacity) {
        size_t new_cap = (list->capacity == 0) ? 16 : list->capacity * 2;
        entry_t *new_entries = realloc(list->entries, new_cap * sizeof(entry_t));
        if (!new_entries) return;
        list->entries = new_entries;
        list->capacity = new_cap;
    }

    entry_t *e = &list->entries[list->count];
    e->name = strdup(name);
    e->path = path ? strdup(path) : strdup(name);
    
    if (lstat(e->path, &e->st) == 0) {
        e->stat_valid = true;
        
        /* Chn mc thi gian ph hp theo c */
        if (opts) {
            if (opts->time_kind == TIME_CTIME) e->sel_time = e->st.st_ctime;
            else if (opts->time_kind == TIME_ATIME) e->sel_time = e->st.st_atime;
            else e->sel_time = e->st.st_mtime;
        } else {
            e->sel_time = e->st.st_mtime;
        }

        /* 512-byte blocks chun POSIX */
        list->total_blocks += e->st.st_blocks;
    } else {
        e->stat_valid = false;
        e->sel_time = 0;
    }

    list->count++;
}

bool read_directory(const char *dir_path, entry_list_t *list, const options_t *opts) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "my_ls: %s: %s\n", dir_path, strerror(errno));
        return false;
    }

    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        /* B qua . v .. nu chn -A (almost_all) */
        if (opts->almost_all && !opts->all) {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                continue;
            }
        }
        
        /* B qua tp n (bt u bng .) nu khng bt -a hoc -A */
        if (!opts->all && !opts->almost_all && dp->d_name[0] == '.') {
            continue;
        }

        /* To full path  lstat */
        char full_path[1024];
        if (strcmp(dir_path, "/") == 0) {
            snprintf(full_path, sizeof(full_path), "/%s", dp->d_name);
        } else {
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, dp->d_name);
        }

        add_entry(list, full_path, dp->d_name, opts);
    }

    closedir(dir);
    return true;
}
