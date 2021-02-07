#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "file_utils.h"
#include "akinator.h"

const char *db_name = "akinator.db";

#define ERROR_HANDLER() goto error_handler

#define MAX_COMMAND_LENGTH 10

int main(int argc, char* argv[])
{
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

    akinator_tell("Welcome to Akinator!\n");

    while (true)
    {
        akinator_tell("How can I help you now?\n");
        printf("[guess|add|find|compare|show|exit]\n");

        char command[MAX_COMMAND_LENGTH + 1];
        read_line(command, MAX_NODE_NAME_LENGTH);

        if (strcmp(command, "add") == 0)
        {
            tree_root = akinator_add(tree_root, &pool);
            if (LERR_PRESENT())
            {
                printf("Adding failed!\n"
                    "Info: %s\n", __lerr_str);
                ERROR_HANDLER();
            }
        }
        if (strcmp(command, "guess") == 0)
        {
            tree_root = akinator_guess(tree_root, &pool);
            if (LERR_PRESENT())
            {
                printf("Guessing failed!\n"
                    "Info: %s\n", __lerr_str);
                ERROR_HANDLER();
            }
        }
        if (strcmp(command, "find") == 0)
        {
            akinator_find(tree_root);
            if (LERR_PRESENT())
            {
                printf("Finding failed!\n"
                    "Info: %s\n", __lerr_str);
                ERROR_HANDLER();
            }
        }
        if (strcmp(command, "compare") == 0)
        {
            akinator_compare(tree_root);
            if (LERR_PRESENT())
            {
                printf("Comparing failed!\n"
                    "Info: %s\n", __lerr_str);
                ERROR_HANDLER();
            }
        }
        if (strcmp(command, "show") == 0)
        {
            tree_visualize(tree_root);
            if (LERR_PRESENT())
            {
                printf("Show failed!\n"
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

        if (strcmp(command, "exit") == 0)
        {
            akinator_tell("Bye!\n");
            break;
        }
    }
    
    free(db_buffer);
    pool_destruct(&pool);
    return 0;

error_handler:
    free(db_buffer);
    pool_destruct(&pool);
    return -1;
}