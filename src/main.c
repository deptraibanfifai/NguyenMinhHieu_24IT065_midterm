#include <stdio.h>
#include "options.h"
#include "list.h"

int main(int argc, char *argv[])
{
    options_t opts;
    int first = parse_options(argc, argv, &opts);

    if (first < 0)
        return 1;

    return list_operands(argc - first, argv + first, &opts);
}
