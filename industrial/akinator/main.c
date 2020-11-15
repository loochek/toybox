#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "search_tree.h"

tree_node_t *akinator_add(tree_node_t *node, const char *thing)
{
    if (node == NULL)
    {
        tree_node_t *to_ret = calloc(1, sizeof(tree_node_t));
        strncpy(to_ret->node_name, thing, MAX_NODE_NAME_LENGTH);
        return to_ret;
    }

    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        printf("%s %s?\n", thing, node->node_name);
        char answer[11] = {0};
        scanf("%10s", answer);
        if (strcmp(answer, "y") == 0)
            node->yes_branch = akinator_add(node->yes_branch, thing);
        else if (strcmp(answer, "n") == 0)
            node->no_branch  = akinator_add(node->no_branch , thing);

        return node;
    }
    printf("What is difference between %s and %s?\n", thing, node->node_name);

    tree_node_t *new_branch_node = calloc(1, sizeof(tree_node_t));
    fgetc(stdin);
    fgets(new_branch_node->node_name, MAX_NODE_NAME_LENGTH, stdin);
    new_branch_node->yes_branch = akinator_add(new_branch_node->yes_branch, thing);
    new_branch_node->no_branch  = node;

    return new_branch_node;
}

void akinator_guess(tree_node_t *node)
{
    if (node == NULL)
    {
        printf("I think I'm too dumb. You should teach me!\n");
        return;
    }
    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        printf("Thing you guessed %s?\n", node->node_name);
        char answer[11] = {0};
        scanf("%10s", answer);
        if (strcmp(answer, "y") == 0)
            akinator_guess(node->yes_branch);
        else if (strcmp(answer, "n") == 0)
            akinator_guess(node->no_branch);

        return;
    }

    printf("Is it %s?\n", node->node_name);
    char answer[11] = {0};
    scanf("%10s", answer);
    if (strcmp(answer, "y") == 0)
        printf("Yeah!\n");
    else if (strcmp(answer, "n") == 0)
        printf("God damn it!\n");
}

int main()
{
    tree_node_t *tree_root = tree_create_from_file("akinator.db");
    //tree_root = akinator_add(tree_root, "loochek");
    tree_dump(tree_root, "akinator_modified.db");
    tree_destruct(tree_root);
    return 0;
}