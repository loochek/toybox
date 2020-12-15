#include <stdio.h>
#include <string.h>

#include "assm_gen.h"
#include "string_view.h"
#include "lerror.h"

// code is some dirty, but it's not final variant
// but it works!

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

static int var_table_add(gen_state_t *state, string_view_t *str)
{
    LERR_RESET();

    if (state->table_size == VAR_TABLE_SIZE)
    {
        LERR(LERR_GENERIC, "table is full");
        return -1;
    }

    int offset = state->table_size;

    state->var_table[offset] = *str;
    state->table_size++;

    return offset;
}

static void var_table_clean(gen_state_t *state)
{
    memset(state->var_table, 0, sizeof(string_view_t) * state->table_size);
    state->table_size = 0;
}

void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file);


static void func_decl_add_args(ast_node_t *node, gen_state_t *state)
{
    if (node == NULL)
        return;

    if (node->type == AST_COMPOUND)
    {
        func_decl_add_args(node->left_branch , state);
        func_decl_add_args(node->right_branch, state);
        return;
    }

    var_table_add(state, &node->ident);
}

static void func_call_calc_args(ast_node_t *node, gen_state_t *state, FILE *file)
{
    LERR_RESET();

    if (node == NULL)
        return;

    if (node->type == AST_COMPOUND)
    {
        func_call_calc_args(node->left_branch , state, file);
        if (LERR_PRESENT())
            return;

        func_call_calc_args(node->right_branch, state, file);
        return;
    }

    assm_gen_rec(node, state, file);
    if (LERR_PRESENT())
        return;

    int offset = var_table_add(state, &(string_view_t){"fictive", 7});
    if (LERR_PRESENT())
        return;

    fprintf(file, "pop [rax+%d]\n", offset);
}

void assm_gen(ast_node_t *ast_root, const char *file_name)
{
    FILE *file = fopen(file_name, "w");

    gen_state_t state = {0};
    state.var_table  = calloc(VAR_TABLE_SIZE, sizeof(string_view_t));
    state.table_size = 0;

    assm_gen_rec(ast_root, &state, file);

    free(state.var_table);
    fclose(file);
}

#define BINARY_OP_TEMPLATE(NODE_TYPE, OPCODE)           \
    case NODE_TYPE:                                     \
    {                                                   \
        assm_gen_rec(node->left_branch , state, file);  \
        if (LERR_PRESENT())                             \
            break;                                      \
                                                        \
        assm_gen_rec(node->right_branch, state, file);  \
        if (LERR_PRESENT())                             \
            break;                                      \
                                                        \
        fprintf(file, #OPCODE "\n");                    \
        break;                                          \
    }

#define COMPARE_OP_TEMPLATE(NODE_TYPE, OPCODE)                         \
    case NODE_TYPE:                                                    \
    {                                                                  \
        assm_gen_rec(node->left_branch , state, file);                 \
        if (LERR_PRESENT())                                            \
            break;                                                     \
                                                                       \
        assm_gen_rec(node->right_branch, state, file);                 \
        if (LERR_PRESENT())                                            \
            break;                                                     \
                                                                       \
        size_t uid = state->uid_cnt++;                                 \
                                                                       \
        fprintf(file, #OPCODE " " #OPCODE "_true_%zu\n"                \
                      "push 0\n"                                       \
                      "jmp " #OPCODE "_final_%zu\n"                    \
                      #OPCODE "_true_%zu:\n"                           \
                      "push 1\n"                                       \
                      #OPCODE "_final_%zu:\n", uid, uid, uid, uid);    \
        break;                                                         \
    }                                                                  \

void assm_gen_rec(ast_node_t *node, gen_state_t *state, FILE *file)
{
    LERR_RESET();

    if (node == NULL)
        return;
        
    switch (node->type)
    {

    case AST_NUMBER:
    {
        fprintf(file, "push %d\n", node->number);
        break;
    }

    case AST_IDENTIFIER:
    {
        int offset = var_lookup(state, &node->ident);
        if (offset < 0)
        {
            LERR(LERR_ASSM_GEN, "unknown identifier %.*s", node->ident.length, node->ident.str);
            break;
        }

        fprintf(file, "push [rax+%d]\n", offset);
        break;
    }

    BINARY_OP_TEMPLATE(AST_OPER_ADD, add)
    BINARY_OP_TEMPLATE(AST_OPER_SUB, sub)
    BINARY_OP_TEMPLATE(AST_OPER_MUL, mul)
    BINARY_OP_TEMPLATE(AST_OPER_DIV, div)

    COMPARE_OP_TEMPLATE(AST_OPER_LESS  , jl)
    COMPARE_OP_TEMPLATE(AST_OPER_MORE  , jg)
    COMPARE_OP_TEMPLATE(AST_OPER_ELESS , jle)
    COMPARE_OP_TEMPLATE(AST_OPER_EMORE , jge)
    COMPARE_OP_TEMPLATE(AST_OPER_EQUAL , je)
    COMPARE_OP_TEMPLATE(AST_OPER_NEQUAL, jne)

    case AST_OPER_ASSIGN:
    {
        assm_gen_rec(node->right_branch, state, file);

        ast_node_t *var_name = node->left_branch;

        int offset = var_lookup(state, &var_name->ident);
        if (offset < 0)
        {
            LERR(LERR_ASSM_GEN, "unknown identifier %.*s", var_name->ident.length, var_name->ident.str);
            break;
        }

        fprintf(file, "pop [rax+%d]\n", offset);
        break;
    }
    case AST_VAR_DECL:
    {
        int offset = var_table_add(state, &node->left_branch->ident);
        if (LERR_PRESENT())
            break;

        if (node->right_branch != NULL)
        {
            assm_gen_rec(node->right_branch, state, file);
            if (LERR_PRESENT())
                break;

            fprintf(file, "pop [rax+%d]\n", offset);
        }
        break;
    }
    case AST_FUNC_DECL:
    {
        var_table_clean(state);

        ast_node_t *func_name = node->left_branch->left_branch;
        ast_node_t *args_root = node->left_branch->right_branch;
        ast_node_t *func_body = node->right_branch;

        func_decl_add_args(args_root, state);

        fprintf(file, "%.*s:\n", func_name->ident.length, func_name->ident.str);

        assm_gen_rec(func_body, state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "ret\n");
        break;
    }
        

    case AST_COMPOUND:
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);
        break;

    case AST_IF:
    {
        assm_gen_rec(node->left_branch , state, file);
        if (LERR_PRESENT())
            break;

        size_t uid = state->uid_cnt++;
        fprintf(file, "push 0\n"
                      "je if_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "if_skip_%zu:\n", uid);

        break;
    }

    case AST_WHILE:
    {
        size_t uid = state->uid_cnt++;
        fprintf(file, "while_back_%zu:\n", uid);

        assm_gen_rec(node->left_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "push 0\n"
                      "je while_skip_%zu\n", uid);

        assm_gen_rec(node->right_branch , state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "jmp while_back_%zu\n"
                      "while_skip_%zu:\n", uid, uid);

        break;
    }

    case AST_OPER_CALL:
    {
        // save current offset of the stack frame to start new frame from this position 
        size_t args_start = state->table_size;

        // calculate args and put it as fictive variables on the stack frame
        func_call_calc_args(node->right_branch, state, file);
        if (LERR_PRESENT())
            break;

        ast_node_t *func_name = node->left_branch;

        // save rax to CPU stack, move rax to beginning of arguments, call function, restore rax,
        // push returned value
        fprintf(file,
                "push rax\n"
                "push rax\n"
                "push %d\n"
                "add\n"
                "pop rax\n"
                "call %.*s\n"
                "pop rax\n"
                "push rbx\n", args_start, func_name->ident.length, func_name->ident.str);

        // remove fictive variables
        state->table_size = args_start;

        break;
    }

    case AST_RETURN:
    {
        if (node->left_branch != NULL)
        {
            assm_gen_rec(node->left_branch, state, file);
            if (LERR_PRESENT())
                break;

            fprintf(file, "pop rbx\n");
        }
        else
        {
            fprintf(file, "push 0\n"
                          "pop rbx\n");
        }

        fprintf(file, "ret\n");
        
        break;
    }

    case AST_EXPR_STMT:
    {
        // AST_EXPR_STMT is the special node to tell code generator to eat return value of the expression
        // in order to keep CPU stack correct
        
        assm_gen_rec(node->left_branch, state, file);
        if (LERR_PRESENT())
            break;

        fprintf(file, "pop rcx\n");

        break;
    }

    default:
        printf("unknown!\n");
        assm_gen_rec(node->left_branch , state, file);
        assm_gen_rec(node->right_branch, state, file);
        break;
    }
}