#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "helper.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <sql>\n", argv[0]);
        return 1;
    }
    ParseResult result = parse_query(argv[1]);
    if (result.error)
    {
        printf("Error: %s\n", result.error);
        return 1;
    }

    print_parse_tree(result.parse_tree);
    free_parse_tree(result.parse_tree);
    return 0;
}