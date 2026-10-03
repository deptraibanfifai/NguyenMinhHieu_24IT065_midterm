#include <stdio.h>
#include "options.h"

int main(int argc, char *argv[])
{
    options_t opts;
    int first = parse_options(argc, argv, &opts);

    if (first < 0)
        return 1;

    /* In thu de kiem tra */
    printf("long=%d numeric=%d time=%d size=%d recursive=%d dir=%d\n",
           opts.long_fmt, opts.numeric, opts.time_kind,
           opts.size_kind, opts.recursive, opts.dir_as_file);

    for (int i = first; i < argc; i++)
        printf("operand: %s\n", argv[i]);

    return 0;
}
