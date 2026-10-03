#ifndef ENTRY_H
#define ENTRY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include "options.h"

typedef struct {
    char *name;         /* Tn hin th */
    char *path;         /* ng dn y  */
    struct stat st;     /* Thng tin stat */
    bool stat_valid;    /*  stat thnh cng hay cha */
    time_t sel_time;    /* Thi gian chn  sp xp (mtime/ctime/atime) */
} entry_t;

typedef struct {
    entry_t *entries;
    size_t count;
    size_t capacity;
    blkcnt_t total_blocks;
} entry_list_t;

void init_entry_list(entry_list_t *list);
void free_entry_list(entry_list_t *list);
void add_entry(entry_list_t *list, const char *path, const char *name, const options_t *opts);
bool read_directory(const char *dir_path, entry_list_t *list, const options_t *opts);

#endif /* ENTRY_H */
