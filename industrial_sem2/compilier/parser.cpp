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
 * COMP_STMT ::= '{' STMT* '}'
 * IF_STMT   ::= 'if' '('EXPR')' STMT { 'else' STMT }?
 * WHILE_STMT::= 'while' '('EXPR')' STMT
 * RET_STMT  ::= 'return' EXPR? ;
 * 
 * FN_DECL_STMT ::= 'fn' IDNT '('{IDNT{,IDNT}*}+')' COMP_STMT
 * 
 * PRG ::= {FN_DECL_STMT}* LEX_PRG_END
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

static lstatus_t grammar_expr_stmt    (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_var_decl_stmt(ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_if_stmt      (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_while_stmt   (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_ret_stmt     (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_comp_stmt    (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_stmt         (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_fn_decl_stmt (ast_node_t **node_out, parser_state_t *state);
static lstatus_t grammar_prg          (ast_node_t **node_out, parser_state_t *state);

/**
 * helper function to translate lexem operator to AST operator
 */
static ast_node_type_t lex_to_ast(lexem_type_t lexem_type);

lstatus_t ast_build(list_t<lexem_t> *lexems, memory_pool_t<ast_node_t> *pool,
                    compilation_error_t *comp_err, ast_node_t **tree_root_out)
{
    lstatus_t status = LSTATUS_OK;

    list_iter_t lexems_iter_begin = NULLITER;
    LSCHK(list_begin(lexems, &lexems_iter_begin));

    parser_state_t state = { lexems, comp_err, pool, lexems_iter_begin };
    LSCHK(grammar_prg(tree_root_out, &state));

    return LSTATUS_OK;
}

//------------------------------------------------------------------------------------------------------

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
    call_node->row = func_name->row;
    call_node->col = func_name->col;

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
        int oper_row = curr_lexem->row;
        int oper_col = curr_lexem->col;
        NEXT_LEXEM();

        LSCHK(grammar_prim(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        oper_node->row = oper_row;
        oper_node->col = oper_col;
        
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
        int oper_row = curr_lexem->row;
        int oper_col = curr_lexem->col;
        NEXT_LEXEM();

        LSCHK(grammar_mudi(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        oper_node->row = oper_row;
        oper_node->col = oper_col;
        
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
        int oper_row = curr_lexem->row;
        int oper_col = curr_lexem->col;
        NEXT_LEXEM();

        LSCHK(grammar_adsu(&second_arg, state));
        
        ALLOC_NODE(oper_node);
        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        oper_node->row = oper_row;
        oper_node->col = oper_col;
        
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
        int assn_row = curr_lexem->row;
        int assn_col = curr_lexem->col;
        NEXT_LEXEM();

        LSCHK_LOCAL(grammar_expr(&value, state));
            
        ALLOC_NODE(assn_node);
        assn_node->type         = AST_OPER_ASSIGN;
        assn_node->left_branch  = var;
        assn_node->right_branch = value;
        assn_node->row = assn_row;
        assn_node->col = assn_col;
        
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

static lstatus_t grammar_expr_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *expr = nullptr, *marker_node = nullptr;

    LSCHK_LOCAL(grammar_expr(&expr, state));

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_SEMICOLON)
        PARSING_ERROR("expected ;");

    NEXT_LEXEM();

    ALLOC_NODE(marker_node);
    marker_node->type         = AST_EXPR_STMT;
    marker_node->left_branch  = expr;
    marker_node->right_branch = NULL;
    marker_node->row = expr->row;
    marker_node->col = expr->col;

    *node_out = marker_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(expr);
    DESTROY_NODE(marker_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_var_decl_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *ident_node = nullptr, *init_val = nullptr, *decl_node = nullptr;

    int let_row = 0, let_col = 0;

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_KW_LET)
        PARSING_ERROR("expected let keyword");

    let_row = curr_lexem->row;
    let_col = curr_lexem->col;
    NEXT_LEXEM();

    LSCHK_LOCAL(grammar_idnt(&ident_node, state));

    FETCH_LEXEM();
    if (curr_lexem->type == LEX_ASSIGN)
    {
        NEXT_LEXEM();
        LSCHK_LOCAL(grammar_expr(&init_val, state));
    }

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_SEMICOLON)
        PARSING_ERROR("expected ;");

    NEXT_LEXEM();

    ALLOC_NODE(decl_node);
    decl_node->type = AST_VAR_DECL;
    decl_node->left_branch = ident_node;
    decl_node->right_branch = init_val;
    decl_node->row = let_row;
    decl_node->col = let_col;

    *node_out = decl_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(ident_node);
    DESTROY_NODE(init_val);
    DESTROY_NODE(decl_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_if_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *condition = nullptr, *if_body = nullptr, *if_branches = nullptr,
               *if_body_else = nullptr, *if_node = nullptr;

    int if_row = 0, if_col = 0;
    
    FETCH_LEXEM();
    if (curr_lexem->type != LEX_KW_IF)
        PARSING_ERROR("expected if keyword");

    if_row = curr_lexem->row;
    if_col = curr_lexem->col;
    NEXT_LEXEM();

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_OPEN_BRACKET)
        PARSING_ERROR("expected (");

    NEXT_LEXEM();

    LSCHK_LOCAL(grammar_expr(&condition, state));

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_CLOSE_BRACKET)
        PARSING_ERROR("expected )");

    NEXT_LEXEM();

    LSCHK_LOCAL(grammar_stmt(&if_body, state));

    FETCH_LEXEM();
    if (curr_lexem->type == LEX_KW_ELSE)
    {
        NEXT_LEXEM();
        LSCHK_LOCAL(grammar_stmt(&if_body_else, state));
    }

    ALLOC_NODE(if_branches);
    if_branches->type         = AST_IF_BRANCHES;
    if_branches->left_branch  = if_body;
    if_branches->right_branch = if_body_else;

    ALLOC_NODE(if_node);
    if_node->type         = AST_IF;
    if_node->left_branch  = condition;
    if_node->right_branch = if_branches;
    if_node->row = if_row;
    if_node->col = if_col;

    *node_out = if_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(condition);
    DESTROY_NODE(if_body);
    DESTROY_NODE(if_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_while_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *condition = nullptr, *while_body = nullptr, *while_node = nullptr;

    int while_row = 0, while_col = 0;
    
    FETCH_LEXEM();
    if (curr_lexem->type != LEX_KW_WHILE)
        PARSING_ERROR("expected while keyword");

    while_row = curr_lexem->row;
    while_col = curr_lexem->col;
    NEXT_LEXEM();

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_OPEN_BRACKET)
        PARSING_ERROR("expected (");

    NEXT_LEXEM();

    LSCHK_LOCAL(grammar_expr(&condition, state));

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_CLOSE_BRACKET)
        PARSING_ERROR("expected )");

    NEXT_LEXEM();

    LSCHK_LOCAL(grammar_stmt(&while_body, state));

    ALLOC_NODE(while_node);
    while_node->type         = AST_WHILE;
    while_node->left_branch  = condition;
    while_node->right_branch = while_body;
    while_node->row = while_row;
    while_node->col = while_col;

    *node_out = while_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(condition);
    DESTROY_NODE(while_body);
    DESTROY_NODE(while_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_ret_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *ret_expr = nullptr, *ret_node = nullptr;

    int ret_row = 0, ret_col = 0;

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_KW_RETURN)
        PARSING_ERROR("expected return keyword");

    ret_row = curr_lexem->row;
    ret_col = curr_lexem->col;
    NEXT_LEXEM();

    TRY_TO_PARSE(grammar_expr(&ret_expr, state), {});

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_SEMICOLON)
        PARSING_ERROR("expected ;");

    NEXT_LEXEM();

    ALLOC_NODE(ret_node);
    ret_node->type = AST_RETURN;
    ret_node->left_branch  = ret_expr;
    ret_node->right_branch = NULL;
    ret_node->row = ret_row;
    ret_node->col = ret_col;

    *node_out = ret_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(ret_expr);
    DESTROY_NODE(ret_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *stmt = nullptr;

    TRY_TO_PARSE(grammar_comp_stmt(&stmt, state),
    {
        *node_out = stmt;
        return LSTATUS_OK;
    });

    TRY_TO_PARSE(grammar_var_decl_stmt(&stmt, state),
    {
        *node_out = stmt;
        return LSTATUS_OK;
    });

    TRY_TO_PARSE(grammar_if_stmt(&stmt, state),
    {
        *node_out = stmt;
        return LSTATUS_OK;
    });

    TRY_TO_PARSE(grammar_while_stmt(&stmt, state),
    {
        *node_out = stmt;
        return LSTATUS_OK;
    });

    TRY_TO_PARSE(grammar_ret_stmt(&stmt, state),
    {
        *node_out = stmt;
        return LSTATUS_OK;
    });

    LSCHK_LOCAL(grammar_expr_stmt(&stmt, state));
    
    *node_out = stmt;
    return LSTATUS_OK;

error_handler:
    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_comp_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *subtree_root = nullptr, *stmt = nullptr, *comp_node = nullptr, *scope_node = nullptr;
    
    FETCH_LEXEM();
    if (curr_lexem->type != LEX_COMPOUND_BEG)
        PARSING_ERROR("expected {");

    NEXT_LEXEM();

    while (({ FETCH_LEXEM(); curr_lexem->type != LEX_COMPOUND_END; }))
    {
        LSCHK_LOCAL(grammar_stmt(&stmt, state));

        if (subtree_root == nullptr)
        {
            subtree_root = stmt;
            stmt = nullptr;
            continue;
        }

        ALLOC_NODE(comp_node);
        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = subtree_root;
        comp_node->right_branch = stmt;

        subtree_root = comp_node;

        // to avoid double-free
        stmt = NULL; comp_node = NULL;
    }

    NEXT_LEXEM();

    ALLOC_NODE(scope_node);
    scope_node->type        = AST_SCOPE;
    scope_node->left_branch = subtree_root;

    *node_out = scope_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(subtree_root);
    DESTROY_NODE(stmt);
    DESTROY_NODE(comp_node);
    DESTROY_NODE(scope_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_fn_decl_stmt(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *args_root = nullptr, *arg = nullptr, *comp_node = nullptr, 
               *func_body = nullptr, *decl_node = nullptr, *decl_head_node = nullptr;

    int fn_row = 0, fn_col = 0;
    
    FETCH_LEXEM();
    if (curr_lexem->type != LEX_KW_FN)
        PARSING_ERROR("expected fn keyword");

    fn_row = curr_lexem->row;
    fn_col = curr_lexem->col;
    NEXT_LEXEM();

    ALLOC_NODE(decl_head_node);
    decl_head_node->type = AST_FUNC_HEAD;
    LSCHK_LOCAL(grammar_idnt(&decl_head_node->left_branch, state));

    FETCH_LEXEM();
    if (curr_lexem->type != LEX_OPEN_BRACKET)
        PARSING_ERROR("expected (");

    NEXT_LEXEM();

    while (({ FETCH_LEXEM(); curr_lexem->type != LEX_CLOSE_BRACKET; }))
    {
        // if not first arg, require comma
        if (args_root != nullptr)
        {
            FETCH_LEXEM();
            if (curr_lexem->type != LEX_COMMA)
                PARSING_ERROR("expected ,");

            NEXT_LEXEM();
        }

        LSCHK_LOCAL(grammar_idnt(&arg, state));

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
        arg = NULL; comp_node = NULL;
    }

    NEXT_LEXEM();

    decl_head_node->right_branch = args_root;
    args_root = nullptr;

    LSCHK_LOCAL(grammar_comp_stmt(&func_body, state));

    ALLOC_NODE(decl_node);
    decl_node->type         = AST_FUNC_DECL;
    decl_node->left_branch  = decl_head_node;
    decl_node->right_branch = func_body;
    decl_node->row = fn_row;
    decl_node->col = fn_col;

    *node_out = decl_node;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(arg);
    DESTROY_NODE(comp_node);
    DESTROY_NODE(decl_head_node);
    DESTROY_NODE(args_root);
    DESTROY_NODE(func_body);
    DESTROY_NODE(decl_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

static lstatus_t grammar_prg(ast_node_t **node_out, parser_state_t *state)
{
    lstatus_t status = LSTATUS_OK;

    lexem_t *curr_lexem = nullptr;
    list_iter_t old_lexem_iter = state->curr_lexem_iter;

    ast_node_t *subtree_root = nullptr, *func_node = nullptr, *comp_node = nullptr;

    while (({ FETCH_LEXEM(); curr_lexem->type != LEX_PRG_END; }))
    {
        LSCHK_LOCAL(grammar_fn_decl_stmt(&func_node, state));
        
        if (subtree_root == nullptr)
        {
            subtree_root = func_node;
            func_node = nullptr;
            continue;
        }

        ALLOC_NODE(comp_node);
        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = subtree_root;
        comp_node->right_branch = func_node;

        subtree_root = comp_node;

        // to avoid double-free
        func_node = nullptr; comp_node = nullptr;
    }

    NEXT_LEXEM();

    *node_out = subtree_root;
    return LSTATUS_OK;

error_handler:
    DESTROY_NODE(subtree_root);
    DESTROY_NODE(func_node);
    DESTROY_NODE(comp_node);

    state->curr_lexem_iter = old_lexem_iter;
    return status;
}

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