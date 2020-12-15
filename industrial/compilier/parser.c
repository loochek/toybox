#include "parser.h"
#include "lerror.h"

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
 * PRG ::= FN_DECL_STMT+ LEX_PRG_END
 */

// aliases
#define LEXEM(OFFSET) state->lexems[state->curr_offset + OFFSET]
#define LERR_PARSING_PRESENT() (__lerrno == LERR_PARSING)
#define ERROR_HANDLER() goto error_handler
#define ERROR_CHECK() if (LERR_PRESENT()) ERROR_HANDLER();

typedef struct
{
    lexem_t *lexems;
    size_t   curr_offset;
} parser_state_t;

static ast_node_t *grammar_idnt(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_num (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_fncl(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_prim(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_mudi(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_adsu(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_cmp (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_assn(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_expr(parser_state_t *state, node_pool_t *pool);

static ast_node_t *grammar_expr_stmt    (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_if_stmt      (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_while_stmt   (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_ret_stmt     (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_comp_stmt    (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_stmt         (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_fn_decl_stmt (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_prg          (parser_state_t *state, node_pool_t *pool);

// helper function to translate lexem operator to AST operator
static ast_node_type_t lex_to_ast(lexem_type_t lexem_type);

////////// main function

ast_node_t *ast_build(lexem_t *lexems, node_pool_t *pool)
{
    parser_state_t state = { lexems, 0 };
    ast_node_t    *tree_root = grammar_prg(&state, pool);

    return tree_root;
}

//////////

static ast_node_t *grammar_idnt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    if (LEXEM(0).type != LEX_IDENTIFIER)
    {
        LERR(LERR_PARSING, "expected identifier");
        return NULL;
    }

    ast_node_t *node = node_pool_claim(pool);
    if (node == NULL)
        return NULL;

    node->type  = AST_IDENTIFIER;
    node->ident = LEXEM(0).ident;

    state->curr_offset++;

    return node;
}

static ast_node_t *grammar_num(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    if (LEXEM(0).type != LEX_NUMBER)
    {
        LERR(LERR_PARSING, "expected number constant");
        return NULL;
    }

    ast_node_t *node = node_pool_claim(pool);
    if (node == NULL)
        return NULL;

    node->type   = AST_NUMBER;
    node->number = LEXEM(0).value;

    state->curr_offset++;

    return node;
}

static ast_node_t *grammar_fncl(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *call_node = NULL, *args_root = NULL, *arg = NULL, *comp_node = NULL, *func_name = NULL;

    func_name = grammar_idnt(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type != LEX_OPEN_BRACKET)
    {
        LERR(LERR_PARSING, "expected (");
        ERROR_HANDLER();
    }
    
    state->curr_offset++;

    while (LEXEM(0).type != LEX_CLOSE_BRACKET)
    {
        // if not first arg, require comma
        if (args_root != NULL)
        {
            if (LEXEM(0).type != LEX_COMMA)
            {
                LERR(LERR_PARSING, "expected ,");
                ERROR_HANDLER();
            }

            state->curr_offset++;
        }

        arg = grammar_expr(state, pool);
        ERROR_CHECK();

        if (args_root == NULL)
        {
            args_root = arg;
            continue;
        }

        comp_node = node_pool_claim(pool);
        ERROR_CHECK();

        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = args_root;
        comp_node->right_branch = arg;

        args_root = comp_node;

        // to avoid double-free
        arg = NULL; comp_node = NULL;
    }

    state->curr_offset++;

    call_node = node_pool_claim(pool);
    ERROR_CHECK();

    call_node->type = AST_OPER_CALL;
    call_node->left_branch  = func_name;
    call_node->right_branch = args_root;

    return call_node;

error_handler:
    ast_destroy(call_node, pool);
    ast_destroy(func_name, pool);
    ast_destroy(args_root, pool);
    ast_destroy(arg      , pool);
    ast_destroy(comp_node, pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_prim(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *expr = NULL;

    if (LEXEM(0).type == LEX_OPEN_BRACKET)
    {
        state->curr_offset++;

        expr = grammar_expr(state, pool);
        ERROR_CHECK();
        
        if (LEXEM(0).type != LEX_CLOSE_BRACKET)
        {
            LERR(LERR_PARSING, "expected closing bracket");
            ERROR_HANDLER();
        }
        
        state->curr_offset++;

        return expr;
    }
    
    ast_node_t *fncl = grammar_fncl(state, pool);
    if (!LERR_PARSING_PRESENT())
        return fncl;
    else
        LERR_RESET();

    ast_node_t *num = grammar_num(state, pool);
    if (!LERR_PARSING_PRESENT())
        return num;
    else
        LERR_RESET();
    
    return grammar_idnt(state, pool);

error_handler:
    ast_destroy(expr, pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_mudi(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL, *second_arg = NULL, *oper_node = NULL;

    subtree_root = grammar_prim(state, pool);
    ERROR_CHECK();

    while (LEXEM(0).type == LEX_OPER_MUL ||
           LEXEM(0).type == LEX_OPER_DIV ||
           LEXEM(0).type == LEX_OPER_MOD)
    {
        ast_node_type_t oper_type = lex_to_ast(LEXEM(0).type);
        state->curr_offset++;

        second_arg = grammar_prim(state, pool);
        ERROR_CHECK();
            
        oper_node = node_pool_claim(pool);
        ERROR_CHECK();

        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
    }

    return subtree_root;

error_handler:
    ast_destroy(subtree_root, pool);
    ast_destroy(second_arg  , pool);
    ast_destroy(oper_node   , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_adsu(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL, *second_arg = NULL, *oper_node = NULL;

    subtree_root = grammar_mudi(state, pool);
    ERROR_CHECK();

    while (LEXEM(0).type == LEX_OPER_ADD ||
           LEXEM(0).type == LEX_OPER_SUB)
    {
        ast_node_type_t oper_type = lex_to_ast(LEXEM(0).type);
        state->curr_offset++;

        second_arg = grammar_mudi(state, pool);
        ERROR_CHECK();
            
        oper_node = node_pool_claim(pool);
        ERROR_CHECK();

        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
    }

    return subtree_root;

error_handler:
    ast_destroy(subtree_root, pool);
    ast_destroy(second_arg  , pool);
    ast_destroy(oper_node   , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_cmp(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL, *second_arg = NULL, *oper_node = NULL;

    subtree_root = grammar_adsu(state, pool);
    ERROR_CHECK();

    while (LEXEM(0).type == LEX_OPER_EQUAL  ||
           LEXEM(0).type == LEX_OPER_NEQUAL ||
           LEXEM(0).type == LEX_OPER_MORE   ||
           LEXEM(0).type == LEX_OPER_LESS   ||
           LEXEM(0).type == LEX_OPER_EMORE  ||
           LEXEM(0).type == LEX_OPER_ELESS)
    {
        ast_node_type_t oper_type = lex_to_ast(LEXEM(0).type);
        state->curr_offset++;

        second_arg = grammar_adsu(state, pool);
        ERROR_CHECK();
            
        oper_node = node_pool_claim(pool);
        ERROR_CHECK();

        oper_node->type         = oper_type;
        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;
        
        subtree_root = oper_node;

        // to avoid double-free
        second_arg = NULL; oper_node = NULL;
    }

    return subtree_root;

error_handler:
    ast_destroy(subtree_root, pool);
    ast_destroy(second_arg  , pool);
    ast_destroy(oper_node   , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_assn(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *var = NULL, *value = NULL, *assn_node = NULL;

    var = grammar_idnt(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type == LEX_OPER_ASSIGN)
    {
        state->curr_offset++;

        value = grammar_expr(state, pool);
        ERROR_CHECK();
            
        assn_node = node_pool_claim(pool);
        ERROR_CHECK();

        assn_node->type         = AST_OPER_ASSIGN;
        assn_node->left_branch  = var;
        assn_node->right_branch = value;
        
        return assn_node;
    }

    LERR(LERR_PARSING, "expected = for assigment");

error_handler:
    ast_destroy(var      , pool);
    ast_destroy(value    , pool);
    ast_destroy(assn_node, pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_expr(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    ast_node_t *assn = grammar_assn(state, pool);
    if (!LERR_PARSING_PRESENT())
        return assn;
    else
        LERR_RESET();
    
    return grammar_cmp(state, pool);
}

static ast_node_t *grammar_expr_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *expr = NULL, *marker_node = NULL;

    expr = grammar_expr(state, pool);
    ERROR_CHECK();

    if (state->lexems[state->curr_offset].type != LEX_SEMICOLON)
    {
        LERR(LERR_PARSING, "expected ;");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    marker_node = node_pool_claim(pool);
    ERROR_CHECK();

    marker_node->type         = AST_EXPR_STMT;
    marker_node->left_branch  = expr;
    marker_node->right_branch = NULL;

    return marker_node;

error_handler:
    ast_destroy(expr       , pool);
    ast_destroy(marker_node, pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *ident_node = NULL, *init_val = NULL, *decl_node = NULL;
    
    if (LEXEM(0).type != LEX_KW_LET)
    {
        LERR(LERR_PARSING, "expected let keyword");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    ident_node = grammar_idnt(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type == LEX_OPER_ASSIGN)
    {
        state->curr_offset++;

        init_val = grammar_expr(state, pool);
        ERROR_CHECK();
    }

    if (LEXEM(0).type != LEX_SEMICOLON)
    {
        LERR(LERR_PARSING, "expected ;");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    decl_node = node_pool_claim(pool);
    ERROR_CHECK();

    decl_node->type = AST_VAR_DECL;
    decl_node->left_branch = ident_node;
    decl_node->right_branch = init_val;

    return decl_node;

error_handler:
    ast_destroy(ident_node, pool);
    ast_destroy(init_val  , pool);
    ast_destroy(decl_node , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_if_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *condition = NULL, *if_body = NULL, *if_node = NULL;
    
    if (LEXEM(0).type != LEX_KW_IF)
    {
        LERR(LERR_PARSING, "expected if keyword");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    if (LEXEM(0).type != LEX_OPEN_BRACKET)
    {
        LERR(LERR_PARSING, "expected (");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    condition = grammar_expr(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type != LEX_CLOSE_BRACKET)
    {
        LERR(LERR_PARSING, "expected )");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    if_body = grammar_stmt(state, pool);
    ERROR_CHECK();

    if_node = node_pool_claim(pool);
    ERROR_CHECK();

    if_node->type         = AST_IF;
    if_node->left_branch  = condition;
    if_node->right_branch = if_body;

    return if_node;

error_handler:
    ast_destroy(condition, pool);
    ast_destroy(if_body  , pool);
    ast_destroy(if_node  , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_while_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *condition = NULL, *while_body = NULL, *while_node = NULL;
    
    if (LEXEM(0).type != LEX_KW_WHILE)
    {
        LERR(LERR_PARSING, "expected while keyword");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    if (LEXEM(0).type != LEX_OPEN_BRACKET)
    {
        LERR(LERR_PARSING, "expected (");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    condition = grammar_expr(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type != LEX_CLOSE_BRACKET)
    {
        LERR(LERR_PARSING, "expected )");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    while_body = grammar_stmt(state, pool);
    ERROR_CHECK();

    while_node = node_pool_claim(pool);
    ERROR_CHECK();

    while_node->type         = AST_WHILE;
    while_node->left_branch  = condition;
    while_node->right_branch = while_body;

    return while_node;

error_handler:
    ast_destroy(condition , pool);
    ast_destroy(while_body, pool);
    ast_destroy(while_node, pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_ret_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *ret_expr = NULL, *ret_node = NULL;

    if (LEXEM(0).type != LEX_KW_RETURN)
    {
        LERR(LERR_PARSING, "expected return keyword");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    ret_expr = grammar_expr(state, pool);
    if (LERR_PARSING_PRESENT())
        LERR_RESET();
        
    ERROR_CHECK();

    if (LEXEM(0).type != LEX_SEMICOLON)
    {
        LERR(LERR_PARSING, "expected ;");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    ret_node = node_pool_claim(pool);
    ERROR_CHECK();

    ret_node->type = AST_RETURN;
    ret_node->left_branch  = ret_expr;
    ret_node->right_branch = NULL;

    return ret_node;

error_handler:
    ast_destroy(ret_expr, pool);
    ast_destroy(ret_node, pool);

    state->curr_offset = old_offset;
    return NULL;
}


static ast_node_t *grammar_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    ast_node_t *stmt = grammar_comp_stmt(state, pool);
    if (!LERR_PARSING_PRESENT())
        return stmt;
    else
        LERR_RESET();

    stmt = grammar_var_decl_stmt(state, pool);
    if (!LERR_PARSING_PRESENT())
        return stmt;
    else
        LERR_RESET();

    stmt = grammar_if_stmt(state, pool);
    if (!LERR_PARSING_PRESENT())
        return stmt;
    else
        LERR_RESET();

    stmt = grammar_while_stmt(state, pool);
    if (!LERR_PARSING_PRESENT())
        return stmt;
    else
        LERR_RESET();

    stmt = grammar_ret_stmt(state, pool);
    if (!LERR_PARSING_PRESENT())
        return stmt;
    else
        LERR_RESET();

    return grammar_expr_stmt(state, pool);
}

static ast_node_t *grammar_comp_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL, *stmt = NULL, *comp_node = NULL;
    
    if (LEXEM(0).type != LEX_COMPOUND_BEG)
    {
        LERR(LERR_PARSING, "expected {");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    while (LEXEM(0).type != LEX_COMPOUND_END)
    {
        stmt = grammar_stmt(state, pool);
        ERROR_CHECK();

        if (subtree_root == NULL)
        {
            subtree_root = stmt;
            continue;
        }

        comp_node = node_pool_claim(pool);
        ERROR_CHECK();

        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = subtree_root;
        comp_node->right_branch = stmt;

        subtree_root = comp_node;

        // to avoid double-free
        stmt = NULL; comp_node = NULL;
    }

    state->curr_offset++;

    return subtree_root;

error_handler:
    ast_destroy(subtree_root, pool);
    ast_destroy(stmt        , pool);
    ast_destroy(comp_node   , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_fn_decl_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *args_root = NULL, *arg = NULL, *comp_node = NULL, 
               *func_body = NULL, *decl_node = NULL, *decl_comp_node = NULL;
    
    if (LEXEM(0).type != LEX_KW_FN)
    {
        LERR(LERR_PARSING, "expected fn keyword");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    decl_comp_node = node_pool_claim(pool);
    ERROR_CHECK();

    decl_comp_node->type        = AST_COMPOUND;
    decl_comp_node->left_branch = grammar_idnt(state, pool);
    ERROR_CHECK();

    if (LEXEM(0).type != LEX_OPEN_BRACKET)
    {
        LERR(LERR_PARSING, "expected (");
        ERROR_HANDLER();
    }

    state->curr_offset++;

    while (LEXEM(0).type != LEX_CLOSE_BRACKET)
    {
        // if not first arg, require comma
        if (args_root != NULL)
        {
            if (LEXEM(0).type != LEX_COMMA)
            {
                LERR(LERR_PARSING, "expected ,");
                ERROR_HANDLER();
            }

            state->curr_offset++;
        }

        arg = grammar_idnt(state, pool);
        ERROR_CHECK();

        if (args_root == NULL)
        {
            args_root = arg;
            continue;
        }

        comp_node = node_pool_claim(pool);
        ERROR_CHECK();

        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = args_root;
        comp_node->right_branch = arg;

        args_root = comp_node;

        // to avoid double-free
        arg = NULL; comp_node = NULL;
    }

    state->curr_offset++;

    decl_comp_node->right_branch = args_root;
    args_root = NULL;

    func_body = grammar_comp_stmt(state, pool);
    ERROR_CHECK();

    decl_node = node_pool_claim(pool);
    ERROR_CHECK();

    decl_node->type         = AST_FUNC_DECL;
    decl_node->left_branch  = decl_comp_node;
    decl_node->right_branch = func_body;

    return decl_node;

error_handler:
    ast_destroy(arg           , pool);
    ast_destroy(comp_node     , pool);
    ast_destroy(decl_comp_node, pool);
    ast_destroy(args_root     , pool);
    ast_destroy(func_body     , pool);
    ast_destroy(decl_node     , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_t *grammar_prg(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();
    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL, *func_node = NULL, *comp_node = NULL;

    while (LEXEM(0).type != LEX_PRG_END)
    {
        func_node = grammar_fn_decl_stmt(state, pool);
        ERROR_CHECK();
        
        if (subtree_root == NULL)
        {
            subtree_root = func_node;
            continue;
        }

        comp_node = node_pool_claim(pool);
        ERROR_CHECK();

        comp_node->type         = AST_COMPOUND;
        comp_node->left_branch  = subtree_root;
        comp_node->right_branch = func_node;

        subtree_root = comp_node;

        // to avoid double-free
        func_node = NULL; comp_node = NULL;
    }

    state->curr_offset++;

    return subtree_root;

error_handler:
    ast_destroy(subtree_root, pool);
    ast_destroy(func_node   , pool);
    ast_destroy(comp_node   , pool);

    state->curr_offset = old_offset;
    return NULL;
}

static ast_node_type_t lex_to_ast(lexem_type_t lexem_type)
{
    switch (lexem_type)
    {
    case LEX_OPER_ADD:
        return AST_OPER_ADD;
    case LEX_OPER_SUB:
        return AST_OPER_SUB;
    case LEX_OPER_MUL:
        return AST_OPER_MUL;
    case LEX_OPER_DIV:
        return AST_OPER_DIV;
    case LEX_OPER_MOD:
        return AST_OPER_MOD;
    case LEX_OPER_ASSIGN:
        return AST_OPER_ASSIGN;
    case LEX_OPER_EQUAL:
        return AST_OPER_EQUAL;
    case LEX_OPER_NEQUAL:
        return AST_OPER_NEQUAL;
    case LEX_OPER_LESS:
        return AST_OPER_LESS;
    case LEX_OPER_MORE:
        return AST_OPER_MORE;
    case LEX_OPER_ELESS:
        return AST_OPER_ELESS;
    case LEX_OPER_EMORE:
        return AST_OPER_EMORE;
    default:
        return AST_DUMMY;
    }
}