#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "search_tree.h"

#define TYPE int
#define elem_t int
#include "stack/stack.h"
#undef elem_t
#undef TYPE

static void parse_node_name(char *buf, size_t buf_size, size_t *curr_pos, char **node_name);

static inline void tab           (FILE *file, size_t level);
static inline bool is_key_symbol (char c);
static void        skip_until_key(const char *buf, size_t buf_size, size_t *curr_pos);


#define ERROR_HANDLER()   goto error_handler
#define STACK_SEC(method) if ((method) != STACK_OK) ERROR_HANDLER()

tree_node_t *tree_create_from_buffer(char *buf, size_t buf_size, memory_pool_t *pool)
{
    LERR_RESET();

    // recursion but actually no
    // definition stack remembers nodes on the way and contains state of recursion of each node

    my_stack_node definition = {0};

    STACK_SEC(stack_construct_node(&definition, 5));

    // push dummy node - it will be filled in recursion cycle
    STACK_SEC(stack_push_node(&definition, (definition_node_t){ NULL, RELATION_NONE }));

    // current parsing position in the buffer
    size_t curr_pos = 0;
    // return value of recursion cycle - useful for children handling
    tree_node_t *ret_val = NULL;

    while (({size_t stack_size = 0; STACK_SEC(stack_size_node(&definition, &stack_size));  stack_size > 0;}))
    {
        definition_node_t curr_def_node = {0};
        STACK_SEC(stack_top_node(&definition, &curr_def_node));
        tree_node_t *curr_node     = curr_def_node.tree_node;
        relation_t   curr_relation = curr_def_node.relation;

        if (curr_relation == RELATION_NONE)
        {
            // state 1 - enter node

            // as we know, we must create node
            tree_node_t *tree_node = calloc_custom(1, sizeof(tree_node_t), pool);
            if (tree_node == NULL)
                ERROR_HANDLER();

            // try to get node_name
            parse_node_name(buf, buf_size, &curr_pos, &tree_node->node_name);
            if (LERR_PRESENT())
                ERROR_HANDLER();

            // try to get branches if they exist
            // it't not fatal if branches aren't present, just leaf
            skip_until_key(buf, buf_size, &curr_pos);
            if (LERR_PRESENT())
            {
                __lerrno = LERR_NO_ERROR;
                STACK_SEC(stack_pop_node(&definition));
                ret_val = tree_node;
                continue;
            }

            if (buf[curr_pos] != '[')
            {
                STACK_SEC(stack_pop_node(&definition));
                ret_val = tree_node;
                continue;
            }

            // deal with branches

            curr_pos++;

            // go to first branch

            STACK_SEC(stack_pop_node (&definition));
            STACK_SEC(stack_push_node(&definition, (definition_node_t){ tree_node, RELATION_YES }));
            STACK_SEC(stack_push_node(&definition, (definition_node_t){ NULL     , RELATION_NONE }));
            continue;
        }
        if (curr_relation == RELATION_YES)
        {
            // returned from yes branch
            curr_node->yes_branch = ret_val;

            // go to no branch
            STACK_SEC(stack_pop_node (&definition));
            STACK_SEC(stack_push_node(&definition, (definition_node_t){ curr_node, RELATION_NO }));
            STACK_SEC(stack_push_node(&definition, (definition_node_t){ NULL     , RELATION_NONE }));
            continue;
        }
        if (curr_relation == RELATION_NO)
        {
            // returned from no branch
            curr_node->no_branch = ret_val;

            skip_until_key(buf, buf_size, &curr_pos);
            if (LERR_PRESENT())
                ERROR_HANDLER();

            if (buf[curr_pos] != ']')
            {
                LERR(LERR_AKINATOR_PARSE, "expected key symbol ], but got %c", buf[curr_pos]);
                ERROR_HANDLER();
            }

            curr_pos++;

            // exit current node
            STACK_SEC(stack_pop_node(&definition));
            ret_val = curr_node;
            
            continue;
        }
    }

    STACK_SEC(stack_destruct_node(&definition));

    // if extra symbols aren't presented already
    if (curr_pos >= buf_size)
        return ret_val;

    while (!is_key_symbol(buf[curr_pos]))
    {
        curr_pos++;
        if (curr_pos >= buf_size)
            return ret_val;
    }

    LERR(LERR_AKINATOR_PARSE, "extra symbols");
    return NULL;

error_handler:
    STACK_SEC(stack_destruct_node(&definition));
    return NULL;
}

int tree_validate(tree_node_t *tree_root)
{
    LERR_RESET();
    if (tree_root == NULL)
    {
        LERR(LERR_AKINATOR_VALIDATION, "null pointer passed");
        return -1;
    }
    
    my_stack_node definition = {0};
    STACK_SEC(stack_construct_node(&definition, 5));
    STACK_SEC(stack_push_node(&definition, (definition_node_t){ tree_root, RELATION_NONE }));

    while (({size_t stack_size = 0; STACK_SEC(stack_size_node(&definition, &stack_size));  stack_size > 0;}))
    {
        definition_node_t curr_def_node = {0};
        STACK_SEC(stack_top_node(&definition, &curr_def_node));
        tree_node_t *curr_node     = curr_def_node.tree_node;
        relation_t   curr_relation = curr_def_node.relation;

        if (curr_relation == RELATION_NONE)
        {
            if (curr_node->node_name == NULL)
            {
                LERR(LERR_AKINATOR_VALIDATION, "node_name is not present");
                ERROR_HANDLER();
            }

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                STACK_SEC(stack_pop_node (&definition));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node, RELATION_YES } ));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node->yes_branch, RELATION_NONE } ));
                continue;
            }
            else if (curr_node->no_branch == NULL && curr_node->yes_branch == NULL)
            {
                STACK_SEC(stack_pop_node(&definition));
                continue;
            }
            else
            {
                LERR(LERR_AKINATOR_VALIDATION, "only one child is present");
                ERROR_HANDLER();
            }
        }
        if (curr_relation == RELATION_YES)
        {
            STACK_SEC(stack_pop_node (&definition));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node, RELATION_NO } ));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node->no_branch, RELATION_NONE } ));
            continue;
        }
        if (curr_relation == RELATION_NO)
        {
            STACK_SEC(stack_pop_node(&definition));
            continue;
        }
    }

    STACK_SEC(stack_destruct_node(&definition));
    return 0;

error_handler:
    STACK_SEC(stack_destruct_node(&definition));
    return -1;
}

#undef STACK_SEC

#define STACK_SEC(method) if ((method) != STACK_OK) return NULL

tree_node_t* tree_search(tree_node_t* tree_root, const char *thing, my_stack_node *definition)
{
    TREE_CHECK_RET(tree_root, NULL);

    // recursion but actually no
    // definition is a path to found node with relationship information
    // definition also contains the state of recursion, so it's used widely
    
    STACK_SEC(stack_push_node(definition, (definition_node_t){ tree_root, RELATION_NONE } ));

    while (({size_t stack_size = 0; STACK_SEC(stack_size_node(definition, &stack_size));  stack_size > 0;}))
    {
        definition_node_t curr_def_node = {0};
        STACK_SEC(stack_top_node(definition, &curr_def_node));
        tree_node_t *curr_node     = curr_def_node.tree_node;
        relation_t   curr_relation = curr_def_node.relation;

        if (curr_relation == RELATION_NONE)
        {
            // enter node

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                // if it is not a leaf, start to process children
                STACK_SEC(stack_pop_node (definition));
                STACK_SEC(stack_push_node(definition,
                            (definition_node_t){ curr_node, RELATION_YES } ));
                STACK_SEC(stack_push_node(definition,
                            (definition_node_t){ curr_node->yes_branch, RELATION_NONE } ));
            }
            else
            {
                // if we found thing, we want to stop search - simply return, definition will be saved
                // else just go out from the node
                if (strcmp(curr_node->node_name, thing) == 0)
                    return curr_node;
                else
                    STACK_SEC(stack_pop_node(definition));
            }

            continue;
        }
        if (curr_relation == RELATION_YES)
        {
            // returned from the yes branch and didn't find anything
            // go to no branch
            STACK_SEC(stack_pop_node (definition));
            STACK_SEC(stack_push_node(definition,
                        (definition_node_t){ curr_node, RELATION_NO } ));
            STACK_SEC(stack_push_node(definition,
                        (definition_node_t){ curr_node->no_branch, RELATION_NONE } ));
            continue;
        }
        if (curr_relation == RELATION_NO)
        {
            // returned from the no branch and didn't find anything
            // go out from the node
            STACK_SEC(stack_pop_node (definition));
            continue;
        }
    }

    // no search result

    return NULL;
}

#undef ERROR_HANDLER
#undef STACK_SEC

// because normal and error cleanup are the same below

#define ERROR_HANDLER()   goto cleanup
#define STACK_SEC(method) if ((method) != STACK_OK) goto cleanup

void tree_dump(tree_node_t *tree_root, const char *file_name)
{
    LERR_RESET();
    TREE_CHECK_RET(tree_root,);

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        LERR(LERR_FILE_IO, "unable to open file");
        return;
    }

    // "kinda recursion" is the same as tree_search one, so i didn't describe it here
    
    my_stack_node definition = {0};
    STACK_SEC(stack_construct_node(&definition, 5));
    STACK_SEC(stack_push_node(&definition, (definition_node_t){ tree_root, RELATION_NONE }));

    // used for tabbing output file
    size_t current_depth = 0;

    while (({size_t stack_size = 0; STACK_SEC(stack_size_node(&definition, &stack_size));  stack_size > 0;}))
    {
        definition_node_t curr_def_node = {0};
        STACK_SEC(stack_top_node(&definition, &curr_def_node));
        tree_node_t *curr_node     = curr_def_node.tree_node;
        relation_t   curr_relation = curr_def_node.relation;

        if (curr_relation == RELATION_NONE)
        {
            tab(file, current_depth);
            fprintf(file, "\"%s\"\n", curr_node->node_name);

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                STACK_SEC(stack_pop_node (&definition));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node, RELATION_YES } ));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node->yes_branch, RELATION_NONE } ));

                tab(file, current_depth);
                fprintf(file, "[\n");
                current_depth++;

                continue;
            }

            STACK_SEC(stack_pop_node(&definition));

            continue;
        }
        if (curr_relation == RELATION_YES)
        {
            STACK_SEC(stack_pop_node (&definition));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node, RELATION_NO } ));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node->no_branch, RELATION_NONE } ));
            continue;
        }
        if (curr_relation == RELATION_NO)
        {
            STACK_SEC(stack_pop_node(&definition));

            current_depth--;
            tab(file, current_depth);
            fprintf(file, "]\n");
            
            continue;
        }
    }

    fclose(file);

cleanup:
    STACK_SEC(stack_destruct_node(&definition));
    return;
}

void tree_visualize(tree_node_t *tree_root)
{
    LERR_RESET();
    TREE_CHECK_RET(tree_root,);

    FILE *file = fopen("tree.dot", "w");
    if (file == NULL)
    {
        LERR(LERR_FILE_IO, "unable to open file");
        return;
    }

    fprintf(file, "digraph\n{");
    
    my_stack_node definition = {0};
    STACK_SEC(stack_construct_node(&definition, 5));
    STACK_SEC(stack_push_node(&definition, (definition_node_t){ tree_root, RELATION_NONE }));

    // used for tabbing output file
    size_t current_node_id = 1;

    while (({size_t stack_size = 0; STACK_SEC(stack_size_node(&definition, &stack_size));  stack_size > 0;}))
    {
        definition_node_t curr_def_node = {0};
        STACK_SEC(stack_top_node(&definition, &curr_def_node));
        tree_node_t *curr_node     = curr_def_node.tree_node;
        relation_t   curr_relation = curr_def_node.relation;

        if (curr_relation == RELATION_NONE)
        {               
            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                fprintf(file, "%zu [label=\"%s\", style=filled, fillcolor=\"#eba226\"];\n",
                        current_node_id, curr_node->node_name);

                STACK_SEC(stack_pop_node (&definition));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node, RELATION_YES } ));
                STACK_SEC(stack_push_node(&definition,
                            (definition_node_t){ curr_node->yes_branch, RELATION_NONE } ));

                current_node_id *= 2;

                continue;
            }
            
            fprintf(file, "%zu [label=\"%s\", style=filled, fillcolor=\"#db4d73\"];\n",
                        current_node_id, curr_node->node_name);

            current_node_id /= 2;
            STACK_SEC(stack_pop_node(&definition));

            continue;
        }
        if (curr_relation == RELATION_YES)
        {
            STACK_SEC(stack_pop_node (&definition));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node, RELATION_NO } ));
            STACK_SEC(stack_push_node(&definition,
                        (definition_node_t){ curr_node->no_branch, RELATION_NONE } ));

            current_node_id *= 2;
            current_node_id++;

            continue;
        }
        if (curr_relation == RELATION_NO)
        {
            STACK_SEC(stack_pop_node(&definition));
            
            fprintf(file, "%zu->%zu [label=\"yes\"];\n", current_node_id, current_node_id * 2);
            fprintf(file, "%zu->%zu [label=\"no\"];\n" , current_node_id, current_node_id * 2 + 1);

            current_node_id /= 2;

            continue;
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    system("dot tree.dot -Tsvg > tree.svg");
    system("firefox tree.svg");
    
cleanup:
    STACK_SEC(stack_destruct_node(&definition));
    return;
}

#undef ERROR_HANDLER
#undef STACK_SEC

// helper function for parsing
static void parse_node_name(char *buf, size_t buf_size, size_t *curr_pos, char **node_name)
{
    LERR_RESET();

    skip_until_key(buf, buf_size, curr_pos);
    if (LERR_PRESENT())
        return;

    if (buf[*curr_pos] != '"')
    {
        LERR(LERR_AKINATOR_PARSE, "expected key symbol \", but got %c", buf[*curr_pos]);
        return;
    }

    (*curr_pos)++;
    if (*curr_pos >= buf_size)
    {
        LERR(LERR_AKINATOR_END, "unexpected end of buffer");
        return;
    }

    size_t node_name_begin = *curr_pos;

    while (buf[*curr_pos] != '"')
    {
        (*curr_pos)++;
        if (*curr_pos >= buf_size)
        {
            LERR(LERR_AKINATOR_END, "unexpected end of buffer");
            return;
        }
    }

    (*node_name) = buf + node_name_begin;
    buf[*curr_pos] = '\0';

    (*curr_pos)++;
}

// utils

static inline bool is_key_symbol(char c)
{
    return c == '"' || c == '[' || c == ']';
}

static void skip_until_key(const char *buf, size_t buf_size, size_t *curr_pos)
{
    LERR_RESET();

    if (*curr_pos >= buf_size)
    {
        LERR(LERR_AKINATOR_END, "unexpected end of buffer");
        return;
    }

    while (!is_key_symbol(buf[*curr_pos]))
    {
        (*curr_pos)++;
        if (*curr_pos >= buf_size)
        {
            LERR(LERR_AKINATOR_END, "unexpected end of buffer");
            return;
        }
    }
}

// print 4*level spaces into file
static inline void tab(FILE *file, size_t level)
{
    for (size_t i = 0; i < level * 4; i++)
        fputc(' ', file);
}