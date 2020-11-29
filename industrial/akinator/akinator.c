#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "lerror.h"
#include "akinator.h"

#define MAX_CMD_LINE_LENGTH 100
#define MAX_SENTENCE_LENGTH MAX_NODE_NAME_LENGTH * 3

void               speak        (const char *sentence);
static inline bool akinator_ask (const char *question);

static tree_node_t *akinator_put_thing(tree_node_t *node, const char *thing, memory_pool_t *pool);
static void         akinator_describe (my_stack_node *stack, const char *display_name, size_t start_depth);


tree_node_t *akinator_add(tree_node_t *tree_root, memory_pool_t *pool)
{
    TREE_CHECK_RET(tree_root, NULL);

    char thing   [MAX_NODE_NAME_LENGTH + 1] = {0};
    char sentence[MAX_SENTENCE_LENGTH  + 1] = {0};

    akinator_tell("What do you want to add?\n");
    read_line(thing, MAX_NODE_NAME_LENGTH);

    tree_node_t *node = tree_root, *prev_node = NULL;

    while (node != NULL && node->no_branch != NULL && node->yes_branch != NULL)
    {
        snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it correct that %s %s", thing, node->node_name);
        
        prev_node = node;
        if (akinator_ask(sentence))
            node = node->yes_branch;
        else
            node = node->no_branch;
    }

    if (prev_node == NULL)
        return akinator_put_thing(node, thing, pool);

    if (node == prev_node->yes_branch)
        prev_node->yes_branch = akinator_put_thing(node, thing, pool);
    else if (node == prev_node->no_branch)
        prev_node->no_branch  = akinator_put_thing(node, thing, pool);

    return tree_root;
}

tree_node_t *akinator_guess(tree_node_t *tree_root, memory_pool_t *pool)
{
    TREE_CHECK_RET(tree_root, NULL);

    if (tree_root == NULL)
    {
        akinator_tell("Sorry but I don't know anything. You should teach me!\n");
        return NULL;
    }

    char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

    tree_node_t *node = tree_root, *prev_node = NULL;

    while (node->no_branch != NULL && node->yes_branch != NULL)
    {
        snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it correct that thing you guessed %s", node->node_name);
        
        prev_node = node;
        if (akinator_ask(sentence))
            node = node->yes_branch;
        else
            node = node->no_branch;
    }

    snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it %s", node->node_name);
    if (akinator_ask(sentence))
        akinator_tell("Yeah!\n");
    else if (akinator_ask("Damn it! Do you want to teach me about it"))
    {
        char thing[MAX_NODE_NAME_LENGTH + 1] = {0};

        akinator_tell("What was it?\n");
        read_line(thing, MAX_NODE_NAME_LENGTH);

        if (prev_node == NULL)
            return akinator_put_thing(node, thing, pool);

        if (node == prev_node->yes_branch)
            prev_node->yes_branch = akinator_put_thing(node, thing, pool);
        else if (node == prev_node->no_branch)
            prev_node->no_branch  = akinator_put_thing(node, thing, pool);
    }

    return tree_root;
}

#define STACK_SEC(method) if ((method) != STACK_OK) return

void akinator_find(tree_node_t *tree_root)
{
    TREE_CHECK_RET(tree_root,);

    char thing[MAX_NODE_NAME_LENGTH + 1] = {0};

    akinator_tell("What do you want to get information about?\n");
    read_line(thing, MAX_NODE_NAME_LENGTH);

    my_stack_node stack = {0};
    STACK_SEC(stack_construct_node(&stack, 5));

    tree_node_t *result = tree_search(tree_root, thing, &stack);

    if (result == NULL)
        akinator_tell("I don't know about it\n");
    else
    {
        akinator_describe(&stack, thing, 0);
        akinator_tell("that's all I know about it\n");
    }

    STACK_SEC(stack_destruct_node(&stack));
}

#undef STACK_SEC

#define CLEANUP()         goto cleanup
#define STACK_SEC(method) if ((method) != STACK_OK) goto cleanup;

void akinator_compare(tree_node_t *tree_root)
{
    TREE_CHECK_RET(tree_root,);

    char  first_thing[MAX_NODE_NAME_LENGTH + 1] = {0};
    char second_thing[MAX_NODE_NAME_LENGTH + 1] = {0};

    akinator_tell("What do you want to compare?\n");
    read_line(first_thing, MAX_NODE_NAME_LENGTH);
    akinator_tell("What do you want to compare with?\n");
    read_line(second_thing, MAX_NODE_NAME_LENGTH);

    my_stack_node  first_path = {0};
    my_stack_node second_path = {0};
    STACK_SEC(stack_construct_node(&first_path , 5));
    STACK_SEC(stack_construct_node(&second_path, 5));

    tree_node_t *first_result  = tree_search(tree_root, first_thing, &first_path);
    tree_node_t *second_result = tree_search(tree_root, second_thing, &second_path);

    if (first_result == NULL || second_result == NULL)
    {
        akinator_tell("I don't know about these things\n");
        CLEANUP();
    }

    size_t first_size = 0, second_size = 0;
    STACK_SEC(stack_size_node(&first_path,  &first_size));
    STACK_SEC(stack_size_node(&second_path, &second_size));

    size_t depth = 0;
    for (;; depth++)
    {
        definition_node_t first_node = {0}, second_node = {0};
        STACK_SEC(stack_at_node(&first_path , &first_node , depth));
        STACK_SEC(stack_at_node(&second_path, &second_node, depth));

        char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

        if (first_node.relation == second_node.relation)
        {
            if (depth == 0)
                akinator_tell("Both of things ");

            // enough to check only one
            if (first_node.relation == RELATION_YES)
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH, "%s, ",
                            first_node.tree_node->node_name);
                akinator_tell(sentence);
            }
            else
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH, "not %s, ",
                            first_node.tree_node->node_name);
                akinator_tell(sentence);
            }
        }
        else
        {
            if (first_node.relation == RELATION_YES)
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH,
                    "It's true for %s that it %s, which is false for %s\n", 
                    first_result->node_name, first_node.tree_node->node_name, second_result->node_name);
            }
            else
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH,
                    "It's true for %s that it %s, which is false for %s\n", 
                    second_result->node_name, first_node.tree_node->node_name, first_result->node_name);
            }
            
            akinator_tell(sentence);

            break;
        }
    }
    
    depth++;

    if (depth < first_size - 1 || depth < second_size - 1)
    {
        akinator_tell("Anyway:\n");

        akinator_describe(&first_path, first_result->node_name, depth);
        akinator_describe(&second_path, second_result->node_name, depth);

        printf("\n");
    }
    
cleanup:
    stack_destruct_node(&first_path);
    stack_destruct_node(&second_path);

    return;
}

#undef STACK_SEC
#undef CLEANUP

static tree_node_t *akinator_put_thing(tree_node_t *node, const char *thing, memory_pool_t *pool)
{
    if (node == NULL)
    {
        tree_node_t *to_ret = calloc_custom(1, sizeof(tree_node_t), pool);
        if (to_ret == NULL)
            return NULL;

        to_ret->node_name = calloc_custom(MAX_NODE_NAME_LENGTH + 1, sizeof(char), pool);
        if (to_ret->node_name == NULL)
            return NULL;

        strncpy(to_ret->node_name, thing, MAX_NODE_NAME_LENGTH);
        return to_ret;
    }

    if (node->no_branch != NULL && node->yes_branch != NULL)
        return node;

    char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

    snprintf(sentence, MAX_SENTENCE_LENGTH,
            "Please tell me a fact that is true for %s but false for %s\n",
            thing, node->node_name);

    akinator_tell(sentence);
    
    tree_node_t *new_branch_node = calloc_custom(1, sizeof(tree_node_t), pool);
    if (new_branch_node == NULL)
        return NULL;

    new_branch_node->node_name = calloc_custom(MAX_NODE_NAME_LENGTH + 1, sizeof(char), pool);
    if (new_branch_node->node_name == NULL)
        return NULL;

    read_line(new_branch_node->node_name, MAX_NODE_NAME_LENGTH);

    new_branch_node->yes_branch = akinator_put_thing(new_branch_node->yes_branch, thing, pool);
    new_branch_node->no_branch  = node;

    return new_branch_node;
}

#define STACK_SEC(method) if ((method) != STACK_OK) return

// helper fucction to get description from the stack
static void akinator_describe(my_stack_node *stack, const char *display_name, size_t start_depth)
{
    size_t size = 0;
    STACK_SEC(stack_size_node(stack, &size));

    char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

    for (size_t i = start_depth; i < size - 1; i++)
    {
        if (i == start_depth)
        {
            snprintf(sentence, MAX_SENTENCE_LENGTH, "%s ", display_name);
            akinator_tell(sentence);
        }

        definition_node_t node = {0};
        STACK_SEC(stack_at_node(stack, &node, i));

        if (node.relation == RELATION_YES)
            snprintf(sentence, MAX_SENTENCE_LENGTH, "%s, ", node.tree_node->node_name);
        else
            snprintf(sentence, MAX_SENTENCE_LENGTH, "not %s, ", node.tree_node->node_name);

        akinator_tell(sentence);
    }
}

#undef STACK_SEC

void speak(const char *sentence)
{
    char speak_cmd[MAX_SENTENCE_LENGTH + MAX_CMD_LINE_LENGTH + 1];
    snprintf(speak_cmd, MAX_SENTENCE_LENGTH + MAX_CMD_LINE_LENGTH + 1,
             "echo \"%s\" | text2wave -o speech.wav >>/dev/null 2>>/dev/null", sentence);
    system(speak_cmd);
    system("play speech.wav speed 4 >>/dev/null 2>>/dev/null");
}

static inline bool akinator_ask(const char *question)
{
    printf("%s? [y/n]\n", question);
    fflush(stdout);
    speak(question);

    char answer[2] = {0};
    scanf("%1s", answer);
    if (strcmp(answer, "y") == 0)
        return true;
    else
        return false;
}

void akinator_tell(const char *sentence)
{
    printf("%s", sentence);
    fflush(stdout);
    speak(sentence);
}

void read_line(char *buffer, size_t max_length)
{
    printf(">");
    fflush(stdout);
    scanf("\n");
    fgets(buffer, max_length, stdin);
    buffer[strlen(buffer) - 1] = '\0';
}