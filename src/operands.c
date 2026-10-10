#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "print.h"
#include "sort.h"

typedef struct ancestor {
    dev_t dev;
    ino_t ino;
    const struct ancestor *parent;
} ancestor_t;

static void display(const entry_list_t *list, const options_t *opts)
{
    if (opts->long_fmt)
        print_long(list, opts);
    else
        print_short(list, opts);
}

/*
 * Display a directory and recursively visit real subdirectories.
 * Child symbolic links are not followed.
 */
static int walk(const char *path, const options_t *opts, int header,
                int *printed, const ancestor_t *parent)
{
    struct stat st;

    if (stat(path, &st) == -1) {
        fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
        return 1;
    }

    for (const ancestor_t *a = parent; a != NULL; a = a->parent) {
        if (a->dev == st.st_dev && a->ino == st.st_ino) {
            fprintf(stderr, "ls: %s: directory cycle\n", path);
            return 1;
        }
    }

    ancestor_t here = {st.st_dev, st.st_ino, parent};
    entry_list_t list;
    entry_list_init(&list);

    int rc = read_directory(path, &list);
    if (rc < 0) {
        entry_list_free(&list);
        return 1;
    }

    int failed = rc != 0;

    filter_hidden(&list, opts);
    sort_entries(&list, opts);

    if (header) {
        if (*printed)
            putchar('\n');

        print_name(path, opts);
        puts(":");
    }

    print_total(&list, opts);
    display(&list, opts);
    *printed = 1;

    if (opts->recursive) {
        for (size_t i = 0; i < list.count; i++) {
            const entry_t *e = &list.items[i];

            /* Never recurse into "." or "..", even with -a. */
            if (S_ISDIR(e->st.st_mode) &&
                strcmp(e->name, ".") != 0 &&
                strcmp(e->name, "..") != 0) {
                failed |= walk(e->path, opts, 1, printed, &here);
            }
        }
    }

    entry_list_free(&list);
    return failed;
}

int list_operands(int count, char **paths, const options_t *opts)
{
    char *defaults[] = {"."};

    if (count == 0) {
        count = 1;
        paths = defaults;
    }

    entry_list_t files;
    entry_list_t dirs;
    entry_list_init(&files);
    entry_list_init(&dirs);

    int failed = 0;

    for (int i = 0; i < count; i++) {
        struct stat st;

        if (lstat(paths[i], &st) == -1) {
            fprintf(stderr, "ls: %s: %s\n", paths[i], strerror(errno));
            failed = 1;
            continue;
        }

        /*
         * A default short listing follows a directory symlink operand.
         * -d, long format and -F preserve the symbolic link itself.
         */
        if (S_ISLNK(st.st_mode) &&
            !opts->dir_as_file &&
            !opts->long_fmt &&
            !opts->classify) {
            struct stat target;

            if (stat(paths[i], &target) == 0 &&
                S_ISDIR(target.st_mode)) {
                st = target;
            }
        }

        entry_list_t *dest =
            S_ISDIR(st.st_mode) && !opts->dir_as_file ? &dirs : &files;

        int rc = entry_list_add(dest, paths[i], paths[i]);

        if (rc != 0) {
            failed = 1;

            if (rc < 0) {
                fprintf(stderr, "ls: out of memory\n");
                break;
            }
        } else {
            dest->items[dest->count - 1].st = st;
        }
    }

    /* Files and directory operands are sorted separately. */
    sort_entries(&files, opts);
    sort_entries(&dirs, opts);

    display(&files, opts);

    int printed = files.count != 0;

    for (size_t i = 0; i < dirs.count; i++) {
        failed |= walk(dirs.items[i].path, opts,
                       count > 1,
                       &printed, NULL);
    }

    entry_list_free(&files);
    entry_list_free(&dirs);

    if (fflush(stdout) == EOF || ferror(stdout))
        failed = 1;

    return failed ? 1 : 0;
}
