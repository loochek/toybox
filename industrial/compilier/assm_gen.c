#include <stdio.h>

#include "assm_gen.h"
#include "string_view.h"

// rax register always stores current offset of stack frame

#define VAR_TABLE_SIZE 100

typedef struct
{
    string_view_t *var_table;
    size_t         table_size;
    size_t         uid_cnt;
} gen_state_t;

static int var_lookup(gen_state_t *state, string_view_t *str)
{
    for (size_t i = 0; i < state->table_size; i++)
    {
        if (strview_equ(&state->var_table[i], str))
            return i;
    }

    return -1;
}

static void func_decl_gen(ast_node_t *node, gen_state_t *state, FILE *file, bool is_left_branch)
{
    if (node->type == AST_COMPOUND)
    {
        func_decl_gen(node->left_branch , state, file, true);
        func_decl_gen(node->right_branch, state, file, false);
        return;
    }

    if (is_left_branch)
    {
        
    }
}

void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file);

void assm_gen(ast_node_t *ast_root, const char *file_name)
{
    FILE *file = fopen(file_name, "w");

    gen_state_t state = {0};
    state.var_table  = calloc(VAR_TABLE_SIZE, sizeof(string_view_t));
    state.table_size = 0;

    assm_gen_rec(ast_root, &state, file);

    fclose(file);
}

void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file)
{
    if (node == NULL)
        return;
        
    switch (node->type)
    {
    case AST_NUMBER:
        fprintf(file, "    push %d\n", node->number);
        break;

    case AST_IDENTIFIER:
    {
        int offset = var_lookup(state, &node->ident);
        if (offset < 0)
        {
            // TODO: identifier not found
        }

        fprintf(file, "    push [rax+%d]\n", offset);

        break;
    }

    case AST_OPER_ADD:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);

        fprintf(file, "    add\n");
        break;

    case AST_OPER_SUB:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);

        fprintf(file, "    sub\n");
        break;

    case AST_OPER_MUL:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);

        fprintf(file, "    mul\n");
        break;

    case AST_OPER_DIV:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);

        fprintf(file, "    div\n");
        break;

    case AST_OPER_ASSIGN:
    {
        assm_gen_rec(node->right_branch, state, file);

        int offset = var_lookup(state, &node->left_branch->ident);
        if (offset < 0)
        {
            // TODO: identifier not found
        }

        fprintf(file, "    pop [rax+%d]\n", offset);
        break;
    }
    case AST_VAR_DECL:
    {
        size_t var_offset = state->table_size;

        state->var_table[state->table_size] = node->left_branch->ident;
        state->table_size++;

        if (node->right_branch != NULL)
        {
            assm_gen_rec(node->right_branch, state, file);
            fprintf(file, "    pop [rax+%zu]\n", var_offset);
        }
        break;
    }
    case AST_FUNC_DECL:
        
        fprintf(file, "%.*s:\n", node->left_branch->ident.length, node->left_branch->ident.str);
        assm_gen_rec(node->right_branch, state, file);
        fprintf(file, "    ret\n");
        break;

    case AST_COMPOUND:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);
        break;

    case AST_IF:
    {
        assm_gen_rec(node->left_branch , state, file);

        size_t uid = state->uid_cnt++;
        fprintf(file, "    push 0\n"
                      "    je if_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);

        fprintf(file, "if_skip_%zu:\n", uid);

        break;
    }

    case AST_WHILE:
    {
        size_t uid = state->uid_cnt++;
        fprintf(file, "while_back_%zu:\n", uid);

        assm_gen_rec(node->left_branch , state, file);

        fprintf(file, "    push 0\n"
                      "    je while_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);

        fprintf(file, "    jmp while_back_%zu\n"
                      "while_skip_%zu:\n", uid, uid);

        break;
    }

    default:
        printf("unknown!\n");
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);
        break;
    }
}