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
static inline void akinator_tell(const char *sentence);
static inline void read_line     (char *buffer, size_t max_length);

static tree_node_t *akinator_put_thing(tree_node_t *node, const char *thing, memory_pool_t *pool);


tree_node_t *akinator_add(tree_node_t *tree_root, memory_pool_t *pool)
{
    TREE_CHECK_RET(tree_root, NULL);

    char thing   [MAX_NODE_NAME_LENGTH + 1] = {0};
    char sentence[MAX_SENTENCE_LENGTH  + 1] = {0};

    akinator_tell("What do you want to add?");
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
        akinator_tell("Sorry but I don't know anything. You should teach me!");
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
        akinator_tell("Yeah!");
    else if (akinator_ask("Damn it! Do you want to teach me about it"))
    {
        char thing[MAX_NODE_NAME_LENGTH + 1] = {0};

        akinator_tell("What was it?");
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

    akinator_tell("What do you want to get information about?");
    read_line(thing, MAX_NODE_NAME_LENGTH);

    my_stack_node stack = {0};
    STACK_SEC(stack_construct_node(&stack, 5));

    tree_node_t *result = tree_search(tree_root, thing, &stack);

    if (result == NULL)
        akinator_tell("I don't know about it");
    else
    {
        size_t size = 0;
        STACK_SEC(stack_size_node(&stack, &size));

        tree_node_t *prev_node = NULL;
        STACK_SEC(stack_at_node(&stack, &prev_node, 0));

        for (size_t i = 1; i < size; i++)
        {
            tree_node_t *node = NULL;
            STACK_SEC(stack_at_node(&stack, &node, i));

            char sentence[MAX_SENTENCE_LENGTH + 1] = {0};
            if (node == prev_node->yes_branch)
                snprintf(sentence, MAX_SENTENCE_LENGTH, "It's true that %s %s", thing, prev_node->node_name);
            else
                snprintf(sentence, MAX_SENTENCE_LENGTH, "It's false that %s %s", thing, prev_node->node_name);

            akinator_tell(sentence);

            prev_node = node;
        }
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

    akinator_tell("What do you want to compare?");
    read_line(first_thing, MAX_NODE_NAME_LENGTH);
    akinator_tell("What do you want to compare with?");
    read_line(second_thing, MAX_NODE_NAME_LENGTH);

    my_stack_node  first_path = {0};
    my_stack_node second_path = {0};
    STACK_SEC(stack_construct_node(&first_path , 5));
    STACK_SEC(stack_construct_node(&second_path, 5));

    tree_node_t *first_result  = tree_search(tree_root, first_thing, &first_path);
    tree_node_t *second_result = tree_search(tree_root, second_thing, &second_path);

    if (first_result == NULL || second_result == NULL)
    {
        akinator_tell("I don't know about these things");
        CLEANUP();
    }

    size_t first_size = 0, second_size = 0;
    STACK_SEC(stack_size_node(&first_path,  &first_size));
    STACK_SEC(stack_size_node(&second_path, &second_size));

    tree_node_t *common_parent = tree_root;
    for (size_t i = 1; i < first_size && i < second_size; i++)
    {
        tree_node_t *first_node = NULL, *second_node = NULL;
        STACK_SEC(stack_at_node(&first_path , &first_node , i));
        STACK_SEC(stack_at_node(&second_path, &second_node, i));

        char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

        if (first_node == second_node)
        {
            // enough to check only one
            if (first_node == common_parent->yes_branch)
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH, "It's true that both %s", common_parent->node_name);
                akinator_tell(sentence);
            }
            else
            {
                snprintf(sentence, MAX_SENTENCE_LENGTH, "It's false that both %s", common_parent->node_name);
                akinator_tell(sentence);
            }

            common_parent = first_node;
        }
        else
        {
            if (second_node == common_parent->yes_branch)
            {
                tree_node_t *tmp = second_result;
                second_result = first_result;
                first_result = tmp;
            }
            snprintf(sentence, MAX_SENTENCE_LENGTH,
                    "It's true for %s that it %s, which is false for %s", 
                    first_result->node_name, common_parent->node_name, second_result->node_name);
            akinator_tell(sentence);

            break;
        }
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
            "Please tell me a fact that is true for %s but false for %s",
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
    speak(question);

    char answer[2] = {0};
    scanf("%1s", answer);
    if (strcmp(answer, "y") == 0)
        return true;
    else
        return false;
}

static inline void akinator_tell(const char *sentence)
{
    printf("%s\n", sentence);
    speak(sentence);
}

static inline void read_line(char *buffer, size_t max_length)
{
    scanf("\n");
    fgets(buffer, max_length, stdin);
    buffer[strlen(buffer) - 1] = '\0';
}