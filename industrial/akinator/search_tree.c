#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "search_tree.h"

static void parse_node_name    (char *buf, size_t buf_size, size_t *curr_pos, char **node_name);
static void parse_node_branches(char *buf, size_t buf_size, size_t *curr_pos,
                                            tree_node_t **node_ptr, memory_pool_t *pool);
static tree_node_t *parse_node (char *buf, size_t buf_size, size_t *curr_pos, memory_pool_t *pool);

// utils

static inline bool is_key_symbol(char c)
{
    return c == '"' || c == '[' || c == ']';
}

static void skip_until_key(const char *buf, size_t buf_size, size_t *curr_pos)
{
    LERR_RESET();

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

static void parse_node_branches(char *buf, size_t buf_size, size_t *curr_pos,
                                tree_node_t **node_ptr, memory_pool_t *pool)
{
    LERR_RESET();

    skip_until_key(buf, buf_size, curr_pos);
    if (LERR_PRESENT())
        return;

    if (buf[*curr_pos] != '[')
    {
        LERR(LERR_AKINATOR_BRANCH, "expected key symbol [, but got %c", buf[*curr_pos]);
        return;
    }
    (*curr_pos)++;

    (*node_ptr)->yes_branch = parse_node(buf, buf_size, curr_pos, pool);
    if (LERR_PRESENT())
        return;

    (*node_ptr)->no_branch  = parse_node(buf, buf_size, curr_pos, pool);
    if (LERR_PRESENT())
        return;

    skip_until_key(buf, buf_size, curr_pos);
    if (LERR_PRESENT())
        return;

    if (buf[*curr_pos] != ']')
    {
        LERR(LERR_AKINATOR_PARSE, "expected key symbol ], but got %c", buf[*curr_pos]);
        return;
    }

    (*curr_pos)++;
}

static tree_node_t *parse_node(char *buf, size_t buf_size, size_t *curr_pos, memory_pool_t *pool)
{
    LERR_RESET();

    tree_node_t *node = calloc_custom(1, sizeof(tree_node_t), pool);
    if (node == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory");
        return NULL;
    }

    parse_node_name(buf, buf_size, curr_pos, &node->node_name);
    if (LERR_PRESENT())
        return NULL;

    parse_node_branches(buf, buf_size, curr_pos, &node, pool);
    if (__lerrno == LERR_AKINATOR_BRANCH || __lerrno == LERR_AKINATOR_END)
        LERR_RESET();
    
    if (LERR_PRESENT())
        return NULL;

    return node;
}

tree_node_t *tree_create_from_buffer(char *buf, size_t buf_size, memory_pool_t *pool)
{
    LERR_RESET();

    size_t curr_pos = 0;
    tree_node_t *tree_root = parse_node(buf, buf_size, &curr_pos, pool);
    if (LERR_PRESENT())
        return NULL;

    if (curr_pos >= buf_size)
        return tree_root;

    while (!is_key_symbol(buf[curr_pos]))
    {
        curr_pos++;
        if (curr_pos >= buf_size)
            return tree_root;
    }

    LERR(LERR_AKINATOR_PARSE, "extra symbols");
    return NULL;
}

static inline void tab(FILE *file, size_t level)
{
    for (size_t i = 0; i < level * 4; i++)
        fputc(' ', file);
}

static void tree_dump_recursive(FILE *file, tree_node_t *node, size_t current_level)
{
    if (node == NULL)
        return;

    tab(file, current_level);
    fprintf(file, "\"%s\"\n", node->node_name);
    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        tab(file, current_level);
        fprintf(file, "[\n");
        tree_dump_recursive(file, node->yes_branch, current_level + 1);
        tree_dump_recursive(file, node->no_branch, current_level + 1);
        tab(file, current_level);
        fprintf(file, "]\n");
    }
}

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
    tree_dump_recursive(file, tree_root, 0);
    fclose(file);
}

int tree_validate(tree_node_t *tree_root)
{
    LERR_RESET();
    if (tree_root == NULL)
        return 0;

    if (tree_root->yes_branch == NULL && tree_root->no_branch == NULL)
        return 0;

    if (tree_root->yes_branch != NULL && tree_root->no_branch != NULL)
    {
        tree_validate(tree_root->yes_branch);
        if (LERR_PRESENT())
            return -1;
        tree_validate(tree_root->no_branch);
        if (LERR_PRESENT())
            return -1;
            
        return 0;
    }

    LERR(LERR_AKINATOR_VALIDATION, "only one child is present");
    return -1;
}