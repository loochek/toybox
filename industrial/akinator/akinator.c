#include <stdlib.h>
#include <stdio.h>

#include "lerror.h"
#include "akinator.h"

static tree_node_t *akinator_add_rec  (tree_node_t *node, const char *thing);
static void         akinator_guess_rec(tree_node_t *node);

tree_node_t *akinator_add(tree_node_t *node, const char *thing)
{
    TREE_CHECK(node, NULL);
    return akinator_add_rec(node, thing);
}

void akinator_guess(tree_node_t *node)
{
    TREE_CHECK(node,);
    akinator_guess_rec(node);
}

static void akinator_guess_rec(tree_node_t *node)
{
    if (node == NULL)
    {
        printf("I think I'm too dumb. You should teach me!\n");
        return;
    }
    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        printf("Thing you guessed %s [y/n]?\n", node->node_name);
        for (;;)
        {
            char answer[2] = {0};
            scanf("%1s", answer);
            if (strcmp(answer, "y") == 0)
            {
                akinator_guess(node->yes_branch);
                break;
            }
            else if (strcmp(answer, "n") == 0)
            {
                akinator_guess(node->no_branch);
                break;
            }
            printf("I'm waiting for your answer\n");
        }

        return;
    }

    printf("Is it %s [y/n]?\n", node->node_name);
    char answer[2] = {0};
    scanf("%1s", answer);
    if (strcmp(answer, "y") == 0)
        printf("Yeah!\n");
    else if (strcmp(answer, "n") == 0)
        printf("God damn it!\n");
    else
        printf("...\n");
}

static tree_node_t *akinator_add_rec(tree_node_t *node, const char *thing)
{
    if (node == NULL)
    {
        tree_node_t *to_ret = calloc(1, sizeof(tree_node_t));
        strncpy(to_ret->node_name, thing, MAX_NODE_NAME_LENGTH);
        return to_ret;
    }

    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        printf("%s %s [y/n]?\n", thing, node->node_name);
        for (;;)
        {
            char answer[2] = {0};
            scanf("%1s", answer);
            if (strcmp(answer, "y") == 0)
            {
                node->yes_branch = akinator_add(node->yes_branch, thing);
                break;
            }
            else if (strcmp(answer, "n") == 0)
            {
                node->no_branch  = akinator_add(node->no_branch , thing);
                break;
            }
            printf("I'm waiting for your answer\n");
        }

        return node;
    }

    printf("What is difference between %s and %s?\n", thing, node->node_name);

    tree_node_t *new_branch_node = calloc(1, sizeof(tree_node_t));

    fgetc(stdin);
    fgets(new_branch_node->node_name, MAX_NODE_NAME_LENGTH, stdin);
    new_branch_node->node_name[strlen(new_branch_node->node_name) - 1] = '\0';

    new_branch_node->yes_branch = akinator_add(new_branch_node->yes_branch, thing);
    new_branch_node->no_branch  = node;

    return new_branch_node;
}