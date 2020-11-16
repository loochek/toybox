#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "lerror.h"
#include "file_utils.h"
#include "search_tree.h"

static void parse_node_name(const char *buf, size_t buf_size, size_t *curr_pos, char *node_name);
static void parse_node_branches(const char *buf, size_t buf_size, size_t *curr_pos,
                                tree_node_t **node_ptr);
static tree_node_t *parse_node(const char *buf, size_t buf_size, size_t *curr_pos);

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

static void parse_node_name(const char *buf, size_t buf_size, size_t *curr_pos, char *node_name)
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

    if (*curr_pos - node_name_begin > MAX_NODE_NAME_LENGTH)
        strncpy(node_name, buf + node_name_begin, MAX_NODE_NAME_LENGTH);
    else
        strncpy(node_name, buf + node_name_begin, *curr_pos - node_name_begin);

    (*curr_pos)++;
}

static void parse_node_branches(const char *buf, size_t buf_size, size_t *curr_pos,
                                tree_node_t **node_ptr)
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

    (*node_ptr)->yes_branch = parse_node(buf, buf_size, curr_pos);
    if (LERR_PRESENT())
        return;

    (*node_ptr)->no_branch  = parse_node(buf, buf_size, curr_pos);
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

static tree_node_t *parse_node(const char *buf, size_t buf_size, size_t *curr_pos)
{
    LERR_RESET();

    tree_node_t *node = calloc(1, sizeof(tree_node_t));
    if (node == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory");
        return NULL;
    }

    parse_node_name(buf, buf_size, curr_pos, node->node_name);
    if (LERR_PRESENT())
    {
        tree_destruct(node);
        return NULL;
    }

    parse_node_branches(buf, buf_size, curr_pos, &node);
    if (__lerrno != LERR_AKINATOR_PARSE)
        LERR_RESET();
    
    if (LERR_PRESENT())
    {
        tree_destruct(node);
        return NULL;
    }

    return node;
}

tree_node_t *tree_create_from_file(const char *file_name)
{
    LERR_RESET();

    char *buf = NULL;
    int buf_size = create_buffer_from_file(file_name, &buf);
    if (LERR_PRESENT())
        return NULL;

    size_t curr_pos = 0;
    tree_node_t *tree_root = parse_node(buf, buf_size, &curr_pos);
    if (LERR_PRESENT())
    {
        free(buf);
        return NULL;
    }

    if (curr_pos >= buf_size)
    {
        free(buf);
        return tree_root;
    }
    while (!is_key_symbol(buf[curr_pos]))
    {
        curr_pos++;
        if (curr_pos >= buf_size)
        {
            free(buf);
            return tree_root;
        }
    }

    LERR(LERR_AKINATOR_PARSE, "extra symbols");
    free(buf);
    tree_destruct(tree_root);
    return NULL;
}

void tree_destruct(tree_node_t *tree_root)
{
    if (tree_root == NULL)
        return;
        
    tree_destruct(tree_root->yes_branch);
    tree_destruct(tree_root->no_branch);
    free(tree_root);
}

static void tree_dump_recursive(FILE *file, tree_node_t *node)
{
    if (node == NULL)
        return;

    fprintf(file, "\"%s\"\n", node->node_name);
    if (node->no_branch != NULL && node->yes_branch != NULL)
    {
        fprintf(file, "[\n");
        tree_dump_recursive(file, node->yes_branch);
        tree_dump_recursive(file, node->no_branch);
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
    tree_dump_recursive(file, tree_root);
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