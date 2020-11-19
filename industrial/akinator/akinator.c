#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "lerror.h"
#include "akinator.h"

#define MAX_SENTENCE_LENGTH MAX_NODE_NAME_LENGTH + 30

static tree_node_t *akinator_add_recursive  (tree_node_t *node, const char *thing);
static tree_node_t *akinator_guess_recursive(tree_node_t *node);
static tree_node_t *akinator_put_thing      (tree_node_t *node, const char *thing);

void               speak        (const char *sentence);
static inline bool akinator_ask (const char *question);
static inline void akinator_tell(const char *sentence);

tree_node_t *akinator_add(tree_node_t *node)
{
    TREE_CHECK(node, NULL);

    char thing[MAX_NODE_NAME_LENGTH + 1] = {0};

    akinator_tell("What do you want to add?");
    scanf("\n");
    fgets(thing, MAX_NODE_NAME_LENGTH, stdin);
    thing[strlen(thing) - 1] = '\0';

    return akinator_add_recursive(node, thing);
}

void akinator_guess(tree_node_t *node)
{
    TREE_CHECK(node,);
    akinator_guess_recursive(node);
}

static tree_node_t *akinator_guess_recursive(tree_node_t *node)
{
    if (node == NULL)
    {
        akinator_tell("Sorry but I don't know anithing. You should teach me!");
        return NULL;
    }

    char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it correct that thing you guessed %s",
                                                node->node_name);
        
        if (akinator_ask(sentence))
            node->yes_branch = akinator_guess_recursive(node->yes_branch);
        else
            node->no_branch  = akinator_guess_recursive(node->no_branch);

        return node;
    }

    snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it %s", node->node_name);
    if (akinator_ask(sentence))
    {
        akinator_tell("Yeah!");
        return node;
    }
    else if (akinator_ask("Damn it! Do you want to teach me about it"))
    {
        char thing[MAX_NODE_NAME_LENGTH + 1] = {0};

        akinator_tell("What was it?");
        scanf("\n");
        fgets(thing, MAX_NODE_NAME_LENGTH, stdin);
        thing[strlen(thing) - 1] = '\0';
        return akinator_put_thing(node, thing);
    }
    return node;
}

static tree_node_t *akinator_add_recursive(tree_node_t *node, const char *thing)
{
    if (node == NULL)
        return akinator_put_thing(node, thing);

    char sentence[MAX_SENTENCE_LENGTH + 1] = {0};

    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        snprintf(sentence, MAX_SENTENCE_LENGTH, "Is it correct that %s %s",
                                                thing, node->node_name);
        if (akinator_ask(sentence))
            node->yes_branch = akinator_add_recursive(node->yes_branch, thing);
        else
            node->no_branch  = akinator_add_recursive(node->no_branch , thing);

        return node;
    }

    return akinator_put_thing(node, thing);
}

static tree_node_t *akinator_put_thing(tree_node_t *node, const char *thing)
{
    if (node == NULL)
    {
        tree_node_t *to_ret = calloc(1, sizeof(tree_node_t));
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
    
    tree_node_t *new_branch_node = calloc(1, sizeof(tree_node_t));

    scanf("\n");
    fgets(new_branch_node->node_name, MAX_NODE_NAME_LENGTH, stdin);
    new_branch_node->node_name[strlen(new_branch_node->node_name) - 1] = '\0';

    new_branch_node->yes_branch = akinator_put_thing(new_branch_node->yes_branch, thing);
    new_branch_node->no_branch  = node;

    return new_branch_node;
}

// tree_node_t* akinator_search_recursive(tree_node_t* node, const char *thing, my_stack_node *stack)
// {
//     // TODO
//     // if (node == NULL)
//     //     return NULL;
    
//     // if (node->no_branch != NULL && node->yes_branch != NULL)
//     // {
//     //     if (strmp(node->node_name, thing) == 0)
//     //     {

//     //     }
//     // }
// }

void speak(const char *sentence)
{
    char speak_cmd[MAX_SENTENCE_LENGTH + 31];
    snprintf(speak_cmd, MAX_SENTENCE_LENGTH + 31, "echo \"%s\" | festival --tts", sentence);
    system(speak_cmd);
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