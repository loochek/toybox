#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "akinator.h"

const char *db_name = "akinator.db";

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: akinator {add|guess}\n"
               "add   - teach me with new things\n"
               "guess - let me guess something\n");
        return 0;
    }

    tree_node_t *tree_root = tree_create_from_file(db_name);
    if (LERR_PRESENT())
    {
        printf("Can't load the database!\n"
               "Info: %s\n", __lerr_str);
        return -1;
    }

    if (strcmp(argv[1], "add") == 0)
    {
        char thing[MAX_NODE_NAME_LENGTH + 1] = {0};
        printf("What do you want to add?\n");
        fgets(thing, MAX_NODE_NAME_LENGTH, stdin);
        thing[strlen(thing) - 1] = '\0';
        tree_root = akinator_add(tree_root, thing);
        if (LERR_PRESENT())
        {
            printf("Adding failed!\n"
                  "Info: %s\n", __lerr_str);
            return -1;
        }
    }
    if (strcmp(argv[1], "guess") == 0)
    {
        akinator_guess(tree_root);
        if (LERR_PRESENT())
        {
            printf("Guessing failed!\n"
                "Info: %s\n", __lerr_str);
            return -1;
        }
    }

    tree_dump(tree_root, db_name);
    if (LERR_PRESENT())
    {
        printf("Can't save the database!\n"
               "Info: %s\n", __lerr_str);
        return -1;
    }
    
    tree_destruct(tree_root);
    return 0;
}