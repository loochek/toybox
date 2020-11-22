#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "file_utils.h"
#include "akinator.h"

const char *db_name = "akinator.db";

#define ERROR_HANDLER() goto error_handler

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: akinator {add|guess}\n"
               "add   - teach me with new things\n"
               "guess - let me guess something\n");
        return 0;
    }

    memory_pool_t pool = {0};
    pool_construct(&pool);
    if (LERR_PRESENT())
    {
        printf("Can't create memory pool!\n"
               "Info: %s\n", __lerr_str);
        return -1;
    }

    char *db_buffer = NULL;
    int buffer_size = create_buffer_from_file(db_name, &db_buffer);
    if (LERR_PRESENT())
    {
        printf("Can't load the database!\n"
               "Info: %s\n", __lerr_str);
        ERROR_HANDLER();
    }

    tree_node_t *tree_root = tree_create_from_buffer(db_buffer, buffer_size, &pool);
    if (LERR_PRESENT())
    {
        printf("Can't load the database!\n"
               "Info: %s\n", __lerr_str);
        ERROR_HANDLER();
    }
    
    if (strcmp(argv[1], "add") == 0)
    {
        tree_root = akinator_add(tree_root, &pool);
        if (LERR_PRESENT())
        {
            printf("Adding failed!\n"
                  "Info: %s\n", __lerr_str);
            ERROR_HANDLER();
        }
    }
    if (strcmp(argv[1], "guess") == 0)
    {
        akinator_guess(tree_root, &pool);
        if (LERR_PRESENT())
        {
            printf("Guessing failed!\n"
                "Info: %s\n", __lerr_str);
            ERROR_HANDLER();
        }
    }

    tree_dump(tree_root, db_name);
    if (LERR_PRESENT())
    {
        printf("Can't save the database!\n"
               "Info: %s\n", __lerr_str);
        ERROR_HANDLER();
    }
    
    free(db_buffer);
    pool_destruct(&pool);
    return 0;

error_handler:
    free(db_buffer);
    pool_destruct(&pool);
    return -1;
}