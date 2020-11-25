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
#define STACK_SEC(method) if ((method) != STACK_OK) goto error_handler

tree_node_t *tree_create_from_buffer(char *buf, size_t buf_size, memory_pool_t *pool)
{
    LERR_RESET();

    // recursion but actually no
    // stack_rec is the recursion state in each node on the way
    // stack_path remembers nodes on the way

    my_stack_int  stack_rec = {0};
    my_stack_node stack_path = {0};

    STACK_SEC(stack_construct_int (&stack_rec , 5));
    STACK_SEC(stack_construct_node(&stack_path, 5));

    tree_node_t *ret_val  = NULL;
    size_t       curr_pos = 0;

    STACK_SEC(stack_push_int(&stack_rec , 1));

    size_t stack_size = 0;
    while (({STACK_SEC(stack_size_int(&stack_rec, &stack_size));  stack_size > 0;}))
    {
        int curr_branch = 0;
        STACK_SEC(stack_top_int(&stack_rec, &curr_branch));

        if (curr_branch == 1)
        {
            // state 1 - enter node
            tree_node_t *node = calloc_custom(1, sizeof(tree_node_t), pool);
            if (node == NULL)
            {
                LERR(LERR_ALLOC, "unable to allocate memory");
                ERROR_HANDLER();
            }

            // try to get node_name
            parse_node_name(buf, buf_size, &curr_pos, &node->node_name);
            if (LERR_PRESENT())
                ERROR_HANDLER();

            // try to get branches if they exist
            // it't not fatal if branches aren't present, just leaf
            skip_until_key(buf, buf_size, &curr_pos);
            if (LERR_PRESENT())
            {
                __lerrno = LERR_NO_ERROR;
                STACK_SEC(stack_pop_int(&stack_rec));
                ret_val = node;
                continue;
            }

            if (buf[curr_pos] != '[')
            {
                STACK_SEC(stack_pop_int(&stack_rec));
                ret_val = node;
                continue;
            }

            // deal with branches

            curr_pos++;

            // go to first branch

            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 2));
            STACK_SEC(stack_push_node(&stack_path, node));
            STACK_SEC(stack_push_int (&stack_rec, 1));
            continue;
        }
        if (curr_branch == 2)
        {
            // returned from yes branch

            tree_node_t *node = NULL;
            STACK_SEC(stack_top_node(&stack_path, &node));
            node->yes_branch = ret_val;

            // go to no branch

            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 3));
            STACK_SEC(stack_push_int (&stack_rec, 1));
            continue;
        }
        if (curr_branch == 3)
        {
            // returned from no branch

            tree_node_t *node = NULL;
            STACK_SEC(stack_top_node(&stack_path, &node));
            node->no_branch = ret_val;

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
            STACK_SEC(stack_pop_int (&stack_rec));
            STACK_SEC(stack_pop_node(&stack_path));
            ret_val = node;
            
            continue;
        }
    }

    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));

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
    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));
    return NULL;
}

tree_node_t* tree_search(tree_node_t* tree_root, const char *thing, my_stack_node *stack_path)
{
    TREE_CHECK(tree_root, NULL);

    // recursion but actually no
    // stack_rec is the recursion state in each node on the way
    // stack_path also used to remember path to search result
    
    my_stack_int stack_rec = {0};
    STACK_SEC(stack_construct_int(&stack_rec, 5));
    STACK_SEC(stack_push_node    (stack_path, tree_root));
    STACK_SEC(stack_push_int     (&stack_rec, 1));

    tree_node_t *ret_val = NULL;

    size_t stack_size = 0;
    while (({STACK_SEC(stack_size_int(&stack_rec, &stack_size));  stack_size > 0;}))
    {
        tree_node_t *curr_node = NULL;
        STACK_SEC(stack_top_node(stack_path, &curr_node));
        int curr_branch = 0;
        STACK_SEC(stack_top_int(&stack_rec, &curr_branch));

        if (ret_val != NULL)
        {
            // if we found something deeper, just pass it despite current state
            // we want to save path, so we pop only recursion state stack
            STACK_SEC(stack_pop_int(&stack_rec));
            continue;
        }

        if (curr_branch == 1)
        {
            // enter node

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                // if it is not a leaf, start to process children
                // increment state of current node and go into left child
                STACK_SEC(stack_pop_int  (&stack_rec));
                STACK_SEC(stack_push_int (&stack_rec, 2));
                STACK_SEC(stack_push_node(stack_path, curr_node->yes_branch));
                STACK_SEC(stack_push_int (&stack_rec, 1));
            }
            else
            {
                // if we found thing, we want to stop search, else just go out from the node
                if (strcmp(curr_node->node_name, thing) == 0)
                    ret_val = curr_node;
                else
                    STACK_SEC(stack_pop_node(stack_path));

                STACK_SEC(stack_pop_int(&stack_rec));
            }
        }
        if (curr_branch == 2)
        {
            // returned from the left child
            // increment state of current node and go into right child
            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 3));
            STACK_SEC(stack_push_node(stack_path, curr_node->no_branch));
            STACK_SEC(stack_push_int (&stack_rec, 1));
            continue;
        }
        if (curr_branch == 3)
        {
            // go out from the node
            STACK_SEC(stack_pop_int(&stack_rec));
            STACK_SEC(stack_pop_node(stack_path));
            continue;
        }
    }

    STACK_SEC(stack_destruct_int(&stack_rec));
    return ret_val;

error_handler:
    STACK_SEC(stack_destruct_int(&stack_rec));
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
    
    my_stack_int  stack_rec = {0};
    my_stack_node stack_path = {0};
    STACK_SEC(stack_construct_int (&stack_rec , 5));
    STACK_SEC(stack_construct_node(&stack_path, 5));
    STACK_SEC(stack_push_node     (&stack_path, tree_root));
    STACK_SEC(stack_push_int      (&stack_rec , 1));

    size_t stack_size = 0;
    while (({STACK_SEC(stack_size_int(&stack_rec, &stack_size));  stack_size > 0;}))
    {
        tree_node_t *curr_node = NULL;
        STACK_SEC(stack_top_node(&stack_path, &curr_node));
        int curr_branch = 0;
        STACK_SEC(stack_top_int (&stack_rec, &curr_branch));

        if (curr_branch == 3)
        {
            STACK_SEC(stack_pop_int (&stack_rec));
            STACK_SEC(stack_pop_node(&stack_path));
            
            continue;
        }
        if (curr_branch == 1)
        {
            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                STACK_SEC(stack_pop_int  (&stack_rec));
                STACK_SEC(stack_push_int (&stack_rec, 2));
                STACK_SEC(stack_push_node(&stack_path, curr_node->yes_branch));
                STACK_SEC(stack_push_int (&stack_rec, 1));
                continue;
            }
            else if (curr_node->no_branch == NULL && curr_node->yes_branch == NULL)
            {
                STACK_SEC(stack_pop_node(&stack_path));
                STACK_SEC(stack_pop_int (&stack_rec));
                continue;
            }
            else
            {
                LERR(LERR_AKINATOR_VALIDATION, "only one child is present");
                ERROR_HANDLER();
            }
        }
        if (curr_branch == 2)
        {
            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 3));
            STACK_SEC(stack_push_node(&stack_path, curr_node->no_branch));
            STACK_SEC(stack_push_int (&stack_rec, 1));
            continue;
        }
    }

    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));
    return 0;

error_handler:
    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));
    return -1;
}

#undef ERROR_HANDLER
#undef STACK_SEC

// because normal and error cleanup are the same below
#define ERROR_HANDLER()   goto cleanup
#define STACK_SEC(method) if ((method) != STACK_OK) goto cleanup

void tree_dump(tree_node_t *tree_root, const char *file_name)
{
    LERR_RESET();
    TREE_CHECK(tree_root,);

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
        LERR(LERR_FILE_IO, "unable to open file");
        return;
    }

    // "kinda recursion" is the same as tree_search one, so i didn't describe it here
    
    my_stack_int  stack_rec = {0};
    my_stack_node stack_path = {0};
    STACK_SEC(stack_construct_int (&stack_rec , 5));
    STACK_SEC(stack_construct_node(&stack_path, 5));
    STACK_SEC(stack_push_node     (&stack_path, tree_root));
    STACK_SEC(stack_push_int      (&stack_rec , 1));

    // used for tabbing output file
    size_t current_depth = 0;

    size_t stack_size = 0;
    while (({STACK_SEC(stack_size_int(&stack_rec, &stack_size));  stack_size > 0;}))
    {
        tree_node_t *curr_node = NULL;
        STACK_SEC(stack_top_node(&stack_path, &curr_node));
        int curr_branch = 0;
        STACK_SEC(stack_top_int (&stack_rec, &curr_branch));

        if (curr_branch == 3)
        {
            STACK_SEC(stack_pop_int (&stack_rec));
            STACK_SEC(stack_pop_node(&stack_path));

            current_depth--;
            tab(file, current_depth);
            fprintf(file, "]\n");
            
            continue;
        }
        if (curr_branch == 1)
        {
            tab(file, current_depth);
            fprintf(file, "\"%s\"\n", curr_node->node_name);

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                STACK_SEC(stack_pop_int  (&stack_rec));
                STACK_SEC(stack_push_int (&stack_rec, 2));
                STACK_SEC(stack_push_node(&stack_path, curr_node->yes_branch));
                STACK_SEC(stack_push_int (&stack_rec, 1));

                tab(file, current_depth);
                fprintf(file, "[\n");
                current_depth++;

                continue;
            }

            STACK_SEC(stack_pop_node(&stack_path));
            STACK_SEC(stack_pop_int (&stack_rec));

            continue;
        }
        if (curr_branch == 2)
        {
            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 3));
            STACK_SEC(stack_push_node(&stack_path, curr_node->no_branch));
            STACK_SEC(stack_push_int (&stack_rec, 1));
            continue;
        }
    }

    fclose(file);

cleanup:
    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));
    return;
}

void tree_visualize(tree_node_t *tree_root)
{
    LERR_RESET();
    TREE_CHECK(tree_root,);

    FILE *file = fopen("tree.dot", "w");
    if (file == NULL)
    {
        LERR(LERR_FILE_IO, "unable to open file");
        return;
    }

    fprintf(file, "digraph\n{");
    
    my_stack_int  stack_rec = {0};
    my_stack_node stack_path = {0};
    STACK_SEC(stack_construct_int (&stack_rec , 5));
    STACK_SEC(stack_construct_node(&stack_path, 5));
    STACK_SEC(stack_push_node     (&stack_path, tree_root));
    STACK_SEC(stack_push_int      (&stack_rec , 1));

    // used for tabbing output file
    size_t current_node_id = 1;

    size_t stack_size = 0;
    while (({STACK_SEC(stack_size_int(&stack_rec, &stack_size));  stack_size > 0;}))
    {
        tree_node_t *curr_node = NULL;
        STACK_SEC(stack_top_node(&stack_path, &curr_node));
        int curr_branch = 0;
        STACK_SEC(stack_top_int (&stack_rec, &curr_branch));

        if (curr_branch == 3)
        {
            STACK_SEC(stack_pop_int (&stack_rec));
            STACK_SEC(stack_pop_node(&stack_path));
            
            fprintf(file, "%zu->%zu [label=\"yes\"];\n", current_node_id, current_node_id * 2);
            fprintf(file, "%zu->%zu [label=\"no\"];\n" , current_node_id, current_node_id * 2 + 1);

            current_node_id /= 2;

            continue;
        }
        if (curr_branch == 1)
        {
            fprintf(file, "%zu [label=\"%s\"];\n", current_node_id, curr_node->node_name);

            if (curr_node->no_branch != NULL && curr_node->yes_branch != NULL)
            {
                STACK_SEC(stack_pop_int  (&stack_rec));
                STACK_SEC(stack_push_int (&stack_rec, 2));
                STACK_SEC(stack_push_node(&stack_path, curr_node->yes_branch));
                STACK_SEC(stack_push_int (&stack_rec, 1));

                current_node_id *= 2;

                continue;
            }

            current_node_id /= 2;
            
            STACK_SEC(stack_pop_node(&stack_path));
            STACK_SEC(stack_pop_int (&stack_rec));

            continue;
        }
        if (curr_branch == 2)
        {
            STACK_SEC(stack_pop_int  (&stack_rec));
            STACK_SEC(stack_push_int (&stack_rec, 3));
            STACK_SEC(stack_push_node(&stack_path, curr_node->no_branch));
            STACK_SEC(stack_push_int (&stack_rec, 1));

            current_node_id *= 2;
            current_node_id++;

            continue;
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    system("dot tree.dot -Tsvg > tree.svg");
    system("firefox tree.svg");
    
cleanup:
    STACK_SEC(stack_destruct_int (&stack_rec));
    STACK_SEC(stack_destruct_node(&stack_path));
    return;
}

#undef ERROR_HANDLER
#undef STACK_SEC


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

static inline void tab(FILE *file, size_t level)
{
    for (size_t i = 0; i < level * 4; i++)
        fputc(' ', file);
}