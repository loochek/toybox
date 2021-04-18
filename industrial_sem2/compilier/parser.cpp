#include "parser.hpp"

/**
 * Language grammar (functions in this file have same names):
 * main rule - PRG
 * 
 * IDNT ::= [a-zA-Z_][a-zA-Z_0-9]* (aka LEX_IDENTIFIER)
 * NUM  ::= [0-9]+ (aka LEX_NUMBER)
 * FNCL ::= IDNT({EXPR{,EXPR}*}+)
 * PRIM ::= '('EXPR')' | FNCL | NUM | IDNT
 * MUDI ::= PRI{[*\%]PRI}*
 * ADSU ::= MUDI{[+-]MUDI}*
 * CMP  ::= ADSU{['<''>''==''!=''<=''>=']ADSU}*
 * ASSN ::= IDNT=EXPR
 * EXPR ::= ASSN | CMP
 * 
 * EXPR_STMT ::= EXPR;
 * VAR_DECL_STMT ::= 'let' IDNT {'=' EXPR }? ';'
 * STMT ::= {COMP_STMT | EXPR_STMT | IF_STATEMENT | WHILE_STMT | VAR_DECL_STMT | RET_STMT}
 * COMP_STMT ::= '{' STMT+ '}'
 * IF_STMT   ::= 'if' '('EXPR')' STMT
 * WHILE_STMT::= 'while' '('EXPR')' STMT
 * RET_STMT  ::= 'return' EXPR? ;
 * 
 * FN_DECL_STMT ::= 'fn' IDNT '('{IDNT{,IDNT}*}+')' COMP_STMT
 * 
 * PRG ::= {FN_DECL_STMT}+ LEX_PRG_END
 */

struct parser_state_t
{
    list_t<lexem_t>           *lexems;
    compilation_error_t       *comp_err;
    memory_pool_t<ast_node_t> *pool;
    list_iter_t                curr_lexem_iter;
};

static lstatus_t grammar_idnt(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_num (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_fncl(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_prim(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_mudi(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_adsu(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_cmp (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_assn(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_expr(ast_node_t **node_out, parser_state_t *state);

// static ast_node_t *grammar_expr_stmt    (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_if_stmt      (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_while_stmt   (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_ret_stmt     (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_comp_stmt    (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_stmt         (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_fn_decl_stmt (parser_state_t *state, node_pool_t *pool);
// static ast_node_t *grammar_prg          (parser_state_t *state, node_pool_t *pool);

// helper function to translate lexem operator to AST operator
static ast_node_type_t lex_to_ast(lexem_type_t lexem_type);

////////// main function //////////

lstatus_t ast_build(list_t<lexem_t> *lexems, memory_pool_t<ast_node_t> *pool,
                    compilation_error_t *comp_err, ast_node_t **tree_root_out)
{
    lstatus_t status = LSTATUS_OK;

    list_iter_t lexems_iter_begin = NULLITER;
    LSCHK(list_begin(lexems, &lexems_iter_begin));

    parser_state_t state = { lexems, comp_err, pool, lexems_iter_begin };
    LSCHK(grammar_expr(tree_root_out, &state));

    return LSTATUS_OK;
}

///////////////////////////////////

#define LSCHK_LOCAL(expr) { status = expr; if (status != LSTATUS_OK) goto error_handler; }

#define PARSING_ERROR(err_str, ...)                                                               \
{                                                                                                 \
    COMPILATION_ERROR(state->comp_err, curr_lexem->row, curr_lexem->col, err_str, ##__VA_ARGS__); \
    status = LSTATUS_PARSER_FAIL;                                                                 \
    goto error_handler;                                                                           \
}

#define TRY_TO_PARSE(expr, if_success)       \
{                                            \
    status = expr;                           \
    if (status == LSTATUS_OK)                \
        if_success                           \
    else if (status == LSTATUS_PARSER_FAIL)  \
        status = LSTATUS_OK;                 \
    else                                     \
        goto error_handler;                  \
}

#define FETCH_LEXEM()      LSCHK_LOCAL(list_data(state->lexems, state->curr_lexem_iter, &curr_lexem));
#define NEXT_LEXEM()       LSCHK_LOCAL(list_next(state->lexems, &state->curr_lexem_iter));
#define ALLOC_NODE(node)   LSCHK_LOCAL(memory_pool_alloc(state->pool, &node));

#define DESTROY_NODE(node) LSCHK(ast_destroy(node, state->pool));

static lstatus_t grammar_idnt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *node = nullptr;

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_IDENTIFIER)
        PARSING_ERROR("expected identifier");

    ALLOC_NODE(node);

    node->type  = AST_IDENTIFIER;
    node->ident = curr_lexem->ident;
    node->row   = curr_lexem->row;
    node->col   = curr_lexem->col;

    NEXT_LEXEM();

    *node_out = node;
    return LSTATUS_OK;

error_handler:
    state->curr_lexem_iter = old_lexem_iter;

    LSCHK(memory_pool_free(state->pool, node));
    return status;
}

static lstatus_t grammar_num(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *node = nullptr;
    
    FETCH_LEXEM();
    if (curr_lexem->type != LEX_NUMBER)
        PARSING_ERROR("expected number");

    ALLOC_NODE(node);
    node->type   = AST_NUMBER;
    node->number = curr_lexem->number;
    node->row    = curr_lexem->row;
    node->col    = curr_lexem->col;

    NEXT_LEXEM();

    *node_out = node;
    return LSTATUS_OK;

error_handler:
    state->curr_lexem_iter = old_lexem_iter;
    
    LSCHK(memory_pool_free(state->pool, node));
    return status;
}

static lstatus_t grammar_fncl(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *node = nullptr, *call_node = nullptr, *args_root = nullptr,
               *arg = nullptr, *comp_node = nullptr, *func_name = nullptr;

    LSCHK_LOCAL(grammar_idnt(&func_name, state));

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_OPEN_BRACKET)
        PARSING_ERROR("expected (");
    
    NEXT_LEXEM();

    while (({ FETCH_LEXEM(); curr_lexem->type != LEX_CLOSE_BRACKET; }))
    {
        // if not first arg, require comma
        if (args_root != nullptr)
        {
            if (curr_lexem->type != LEX_COMMA)
                PARSING_ERROR("expected ,");

            NEXT_LEXEM();
        }

        LSCHK_LOCAL(grammar_expr(&arg, state));

        if (args_root == nullptr)
        {
            args_root = arg;
            arg = nullptr;
            continue;
        }

        ALLOC_NODE(comp_node);
        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = args_root;
        comp_node->right_branch = arg;

        args_root = comp_node;

        // to avoid double-free
        arg = nullptr; comp_node = nullptr;
    }

    NEXT_LEXEM();

    ALLOC_NODE(call_node);
    call_node->type = AST_CALL;
    call_node->left_branch  = func_name;
    call_node->right_branch = args_root;

    *node_out = call_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(call_node);
    DESTROY_NODE(func_name);
    DESTROY_NODE(args_root);
    DESTROY_NODE(arg);
    DESTROY_NODE(comp_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_prim(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *expr = nullptr, *node = nullptr;

    FETCH_LEXEM();

    if (curr_lexem->type == LEX_OPEN_BRACKET)
    {
        NEXT_LEXEM();

        LSCHK_LOCAL(grammar_expr(&expr, state));
        
        FETCH_LEXEM();
        if (curr_lexem->type != LEX_CLOSE_BRACKET)
            PARSING_ERROR("expected )");

        NEXT_LEXEM();

        *node_out = expr;
        return LSTATUS_OK;
    }
    
    TRY_TO_PARSE(grammar_fncl(&node, state),
    {
        *node_out = node;
        return LSTATUS_OK;
    });

    TRY_TO_PARSE(grammar_num(&node, state),
    {
        *node_out = node;
        return LSTATUS_OK;
    });
    
    LSCHK_LOCAL(grammar_idnt(&node, state));

    *node_out = node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(expr);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_mudi(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *subtree_root = nullptr, *second_arg = nullptr, *oper_node = nullptr;

    LSCHK_LOCAL(grammar_prim(&subtree_root, state));

    while (({FETCH_LEXEM();
             curr_lexem->type == LEX_MUL ||
             curr_lexem->type == LEX_DIV ||
             curr_lexem->type == LEX_MOD;}))
    {
        ast_node_type_t oper_type = lex_to_ast(curr_lexem->type);
        NEXT_LEXEM();

        LSCHK(grammar_prim(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
    }

    *node_out = subtree_root;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(subtree_root);
    DESTROY_NODE(second_arg);
    DESTROY_NODE(oper_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_adsu(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *subtree_root = nullptr, *second_arg = nullptr, *oper_node = nullptr;

    LSCHK_LOCAL(grammar_mudi(&subtree_root, state));

    while (({FETCH_LEXEM();
             curr_lexem->type == LEX_ADD ||
             curr_lexem->type == LEX_SUB;}))
    {
        ast_node_type_t oper_type = lex_to_ast(curr_lexem->type);
        NEXT_LEXEM();

        LSCHK(grammar_mudi(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
    }

    *node_out = subtree_root;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(subtree_root);
    DESTROY_NODE(second_arg);
    DESTROY_NODE(oper_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_cmp(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *subtree_root = nullptr, *second_arg = nullptr, *oper_node = nullptr;

    LSCHK_LOCAL(grammar_adsu(&subtree_root, state));

    while (({FETCH_LEXEM();
             curr_lexem->type == LEX_LESS  ||
             curr_lexem->type == LEX_MORE  ||
             curr_lexem->type == LEX_ELESS ||
             curr_lexem->type == LEX_EMORE ||
             curr_lexem->type == LEX_EQUAL ||
             curr_lexem->type == LEX_NEQUAL;}))
    {
        ast_node_type_t oper_type = lex_to_ast(curr_lexem->type);
        NEXT_LEXEM();

        LSCHK(grammar_adsu(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
        FETCH_LEXEM();
    }

    *node_out = subtree_root;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(subtree_root);
    DESTROY_NODE(second_arg);
    DESTROY_NODE(oper_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_assn(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *var = nullptr, *value = nullptr, *assn_node = nullptr;

    LSCHK_LOCAL(grammar_idnt(&var, state));

    FETCH_LEXEM();
    if (curr_lexem->type == LEX_ASSIGN)
    {
        NEXT_LEXEM();

        LSCHK_LOCAL(grammar_expr(&value, state));
            
        ALLOC_NODE(assn_node);
        assn_node->type         = AST_OPER_ASSIGN;
        assn_node->left_branch  = var;
        assn_node->right_branch = value;
        
        *node_out = assn_node;
        return LSTATUS_OK;
    }

    PARSING_ERROR("expected = for assigment");

error_handler:
    DESTROY_NODE(var);
    DESTROY_NODE(value);
    DESTROY_NODE(assn_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_expr(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *node = nullptr;
    
    TRY_TO_PARSE(grammar_assn(&node, state),
    {
        *node_out = node;
        return LSTATUS_OK;
    });

    LSCHK_LOCAL(grammar_cmp(&node, state));

    *node_out = node;
    return LSTATUS_OK;

error_handler:
    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

// static ast_node_t *grammar_expr_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *expr = NULL, *marker_node = NULL;

//     expr = grammar_expr(state, pool);
//     ERROR_CHECK();

//     if (state->lexems[state->curr_offset].type != LEX_SEMICOLON)
//     {
//         LERR(LERR_PARSING, "expected ;");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     marker_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     marker_node->type         = AST_EXPR_STMT;
//     marker_node->left_branch  = expr;
//     marker_node->right_branch = NULL;

//     return marker_node;

// error_handler:
//     ast_destroy(expr       , pool);
//     ast_destroy(marker_node, pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *ident_node = NULL, *init_val = NULL, *decl_node = NULL;
    
//     if (LEXEM(0).type != LEX_KW_LET)
//     {
//         LERR(LERR_PARSING, "expected let keyword");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     ident_node = grammar_idnt(state, pool);
//     ERROR_CHECK();

//     if (LEXEM(0).type == LEX_OPER_ASSIGN)
//     {
//         state->curr_offset++;

//         init_val = grammar_expr(state, pool);
//         ERROR_CHECK();
//     }

//     if (LEXEM(0).type != LEX_SEMICOLON)
//     {
//         LERR(LERR_PARSING, "expected ;");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     decl_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     decl_node->type = AST_VAR_DECL;
//     decl_node->left_branch = ident_node;
//     decl_node->right_branch = init_val;

//     return decl_node;

// error_handler:
//     ast_destroy(ident_node, pool);
//     ast_destroy(init_val  , pool);
//     ast_destroy(decl_node , pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_if_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *condition = NULL, *if_body = NULL, *if_node = NULL;
    
//     if (LEXEM(0).type != LEX_KW_IF)
//     {
//         LERR(LERR_PARSING, "expected if keyword");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     if (LEXEM(0).type != LEX_OPEN_BRACKET)
//     {
//         LERR(LERR_PARSING, "expected (");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     condition = grammar_expr(state, pool);
//     ERROR_CHECK();

//     if (LEXEM(0).type != LEX_CLOSE_BRACKET)
//     {
//         LERR(LERR_PARSING, "expected )");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     if_body = grammar_stmt(state, pool);
//     ERROR_CHECK();

//     if_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     if_node->type         = AST_IF;
//     if_node->left_branch  = condition;
//     if_node->right_branch = if_body;

//     return if_node;

// error_handler:
//     ast_destroy(condition, pool);
//     ast_destroy(if_body  , pool);
//     ast_destroy(if_node  , pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_while_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *condition = NULL, *while_body = NULL, *while_node = NULL;
    
//     if (LEXEM(0).type != LEX_KW_WHILE)
//     {
//         LERR(LERR_PARSING, "expected while keyword");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     if (LEXEM(0).type != LEX_OPEN_BRACKET)
//     {
//         LERR(LERR_PARSING, "expected (");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     condition = grammar_expr(state, pool);
//     ERROR_CHECK();

//     if (LEXEM(0).type != LEX_CLOSE_BRACKET)
//     {
//         LERR(LERR_PARSING, "expected )");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     while_body = grammar_stmt(state, pool);
//     ERROR_CHECK();

//     while_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     while_node->type         = AST_WHILE;
//     while_node->left_branch  = condition;
//     while_node->right_branch = while_body;

//     return while_node;

// error_handler:
//     ast_destroy(condition , pool);
//     ast_destroy(while_body, pool);
//     ast_destroy(while_node, pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_ret_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *ret_expr = NULL, *ret_node = NULL;

//     if (LEXEM(0).type != LEX_KW_RETURN)
//     {
//         LERR(LERR_PARSING, "expected return keyword");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     ret_expr = grammar_expr(state, pool);
//     if (LERR_PARSING_PRESENT())
//         LERR_RESET();
        
//     ERROR_CHECK();

//     if (LEXEM(0).type != LEX_SEMICOLON)
//     {
//         LERR(LERR_PARSING, "expected ;");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     ret_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     ret_node->type = AST_RETURN;
//     ret_node->left_branch  = ret_expr;
//     ret_node->right_branch = NULL;

//     return ret_node;

// error_handler:
//     ast_destroy(ret_expr, pool);
//     ast_destroy(ret_node, pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }


// static ast_node_t *grammar_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();

//     ast_node_t *stmt = grammar_comp_stmt(state, pool);
//     if (!LERR_PARSING_PRESENT())
//         return stmt;
//     else
//         LERR_RESET();

//     stmt = grammar_var_decl_stmt(state, pool);
//     if (!LERR_PARSING_PRESENT())
//         return stmt;
//     else
//         LERR_RESET();

//     stmt = grammar_if_stmt(state, pool);
//     if (!LERR_PARSING_PRESENT())
//         return stmt;
//     else
//         LERR_RESET();

//     stmt = grammar_while_stmt(state, pool);
//     if (!LERR_PARSING_PRESENT())
//         return stmt;
//     else
//         LERR_RESET();

//     stmt = grammar_ret_stmt(state, pool);
//     if (!LERR_PARSING_PRESENT())
//         return stmt;
//     else
//         LERR_RESET();

//     return grammar_expr_stmt(state, pool);
// }

// static ast_node_t *grammar_comp_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *subtree_root = NULL, *stmt = NULL, *comp_node = NULL, *scope_node = NULL;
    
//     if (LEXEM(0).type != LEX_COMPOUND_BEG)
//     {
//         LERR(LERR_PARSING, "expected {");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     while (LEXEM(0).type != LEX_COMPOUND_END)
//     {
//         stmt = grammar_stmt(state, pool);
//         ERROR_CHECK();

//         if (subtree_root == NULL)
//         {
//             subtree_root = stmt;
//             stmt = NULL;
//             continue;
//         }

//         comp_node = node_pool_claim(pool);
//         ERROR_CHECK();

//         comp_node->type         = AST_COMPOUND;
//         comp_node->left_branch  = subtree_root;
//         comp_node->right_branch = stmt;

//         subtree_root = comp_node;

//         // to avoid double-free
//         stmt = NULL; comp_node = NULL;
//     }

//     state->curr_offset++;

//     scope_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     scope_node->type        = AST_SCOPE;
//     scope_node->left_branch = subtree_root;

//     return scope_node;

// error_handler:
//     ast_destroy(subtree_root, pool);
//     ast_destroy(stmt        , pool);
//     ast_destroy(comp_node   , pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_fn_decl_stmt(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *args_root = NULL, *arg = NULL, *comp_node = NULL, 
//                *func_body = NULL, *decl_node = NULL, *decl_head_node = NULL;
    
//     if (LEXEM(0).type != LEX_KW_FN)
//     {
//         LERR(LERR_PARSING, "expected fn keyword");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     decl_head_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     decl_head_node->type        = AST_FUNC_HEAD;
//     decl_head_node->left_branch = grammar_idnt(state, pool);
//     ERROR_CHECK();

//     if (LEXEM(0).type != LEX_OPEN_BRACKET)
//     {
//         LERR(LERR_PARSING, "expected (");
//         ERROR_HANDLER();
//     }

//     state->curr_offset++;

//     while (LEXEM(0).type != LEX_CLOSE_BRACKET)
//     {
//         // if not first arg, require comma
//         if (args_root != NULL)
//         {
//             if (LEXEM(0).type != LEX_COMMA)
//             {
//                 LERR(LERR_PARSING, "expected ,");
//                 ERROR_HANDLER();
//             }

//             state->curr_offset++;
//         }

//         arg = grammar_idnt(state, pool);
//         ERROR_CHECK();

//         if (args_root == NULL)
//         {
//             args_root = arg;
//             arg = NULL;
//             continue;
//         }

//         comp_node = node_pool_claim(pool);
//         ERROR_CHECK();

//         comp_node->type         = AST_COMPOUND;
//         comp_node->left_branch  = args_root;
//         comp_node->right_branch = arg;

//         args_root = comp_node;

//         // to avoid double-free
//         arg = NULL; comp_node = NULL;
//     }

//     state->curr_offset++;

//     decl_head_node->right_branch = args_root;
//     args_root = NULL;

//     func_body = grammar_comp_stmt(state, pool);
//     ERROR_CHECK();

//     decl_node = node_pool_claim(pool);
//     ERROR_CHECK();

//     decl_node->type         = AST_FUNC_DECL;
//     decl_node->left_branch  = decl_head_node;
//     decl_node->right_branch = func_body;

//     return decl_node;

// error_handler:
//     ast_destroy(arg           , pool);
//     ast_destroy(comp_node     , pool);
//     ast_destroy(decl_head_node, pool);
//     ast_destroy(args_root     , pool);
//     ast_destroy(func_body     , pool);
//     ast_destroy(decl_node     , pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

// static ast_node_t *grammar_prg(parser_state_t *state, node_pool_t *pool)
// {
//     LERR_RESET();
//     size_t old_offset = state->curr_offset;

//     ast_node_t *subtree_root = NULL, *func_node = NULL, *comp_node = NULL;

//     while (LEXEM(0).type != LEX_PRG_END)
//     {
//         func_node = grammar_fn_decl_stmt(state, pool);
//         ERROR_CHECK();
        
//         if (subtree_root == NULL)
//         {
//             subtree_root = func_node;
//             func_node = NULL;
//             continue;
//         }

//         comp_node = node_pool_claim(pool);
//         ERROR_CHECK();

//         comp_node->type         = AST_COMPOUND;
//         comp_node->left_branch  = subtree_root;
//         comp_node->right_branch = func_node;

//         subtree_root = comp_node;

//         // to avoid double-free
//         func_node = NULL; comp_node = NULL;
//     }

//     state->curr_offset++;

//     return subtree_root;

// error_handler:
//     ast_destroy(subtree_root, pool);
//     ast_destroy(func_node   , pool);
//     ast_destroy(comp_node   , pool);

//     state->curr_offset = old_offset;
//     return NULL;
// }

static ast_node_type_t lex_to_ast(lexem_type_t lexem_type)
{
    switch (lexem_type)
    {
    case LEX_ADD:
        return AST_OPER_ADD;
    case LEX_SUB:
        return AST_OPER_SUB;
    case LEX_MUL:
        return AST_OPER_MUL;
    case LEX_DIV:
        return AST_OPER_DIV;
    case LEX_MOD:
        return AST_OPER_MOD;
    case LEX_ASSIGN:
        return AST_OPER_ASSIGN;
    case LEX_EQUAL:
        return AST_OPER_EQUAL;
    case LEX_NEQUAL:
        return AST_OPER_NEQUAL;
    case LEX_LESS:
        return AST_OPER_LESS;
    case LEX_MORE:
        return AST_OPER_MORE;
    case LEX_ELESS:
        return AST_OPER_ELESS;
    case LEX_EMORE:
        return AST_OPER_EMORE;
    default:
        return AST_DUMMY;
    }
}