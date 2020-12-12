#include "parser.h"
#include "lerror.h"

/**
 * Language grammar (functions in parser.c have same names):
 * main rule - PRG
 * 
 * NUM  ::= [0-9]+ (aka LEX_NUMBER)
 * VAR  ::= [a-zA-Z_][a-zA-Z_0-9]* (aka LEX_IDENTIFIER)
 * FNCL ::= LEX_IDENTIFIER()
 * PRIM ::= '('EXPR')' | FNCL | NUM | VAR
 * MUDI ::= PRI{[*\%]PRI}*
 * ADSU ::= MUDI{[+-]MUDI}*
 * CMP  ::= ADSU{['<''>''==''!=']ADSU}*
 * ASSN ::= VAR=EXPR
 * EXPR ::= ASSN | CMP
 * 
 * EXPR_STMT ::= EXPR;
 * VAR_DECL_STMT ::= 'let' LEX_IDENTIFIER {'=' EXPR }? ';'
 * STMT ::= {COMP_STMT | EXPR_STMT | IF_STATEMENT | VAR_DECL_STMT}
 * COMP_STMT ::= '{' STMT+ '}'
 * IF_STMT   ::= 'if' '('EXPR')' { COMP_STMT | STMT }
 * 
 * FN_DECL_STMT ::= 'fn' LEX_IDENTIFIER '(' ')' COMP_STMT
 * 
 * PRG ::= FN_DECL_STMT+ LEX_PRG_END
 */

typedef struct
{
    lexem_t *lexems;
    size_t   curr_offset;
} parser_state_t;

static ast_node_t *grammar_num (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_var (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_fncl(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_prim(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_mudi(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_adsu(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_cmp (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_assn(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_expr(parser_state_t *state, node_pool_t *pool);

static ast_node_t *grammar_expr_stmt    (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_comp_stmt    (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_stmt         (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_fn_decl_stmt (parser_state_t *state, node_pool_t *pool);
static ast_node_t *grammar_prg          (parser_state_t *state, node_pool_t *pool);

ast_node_t *ast_build(lexem_t *lexems, node_pool_t *pool)
{
    parser_state_t state = { lexems, 0 };
    ast_node_t    *tree_root = grammar_prg(&state, pool);

    return tree_root;
}

static ast_node_t *grammar_num(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    lexem_t curr_lexem = state->lexems[state->curr_offset];

    if (curr_lexem.type == LEX_NUMBER)
    {
        ast_node_t *node = node_pool_claim(pool);
        if (node == NULL)
            return NULL;

        node->type   = AST_NUMBER;
        node->number = curr_lexem.value;

        state->curr_offset++;

        return node;
    }

    LERR(LERR_PARSING, "expected number constant");

    return NULL;
}

static ast_node_t *grammar_var(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    lexem_t curr_lexem = state->lexems[state->curr_offset];

    if (curr_lexem.type == LEX_IDENTIFIER)
    {
        ast_node_t *node = node_pool_claim(pool);
        if (node == NULL)
            return NULL;

        node->type         = AST_IDENTIFIER;
        node->ident        = curr_lexem.begin;
        node->ident_length = curr_lexem.length;

        state->curr_offset++;

        return node;
    }

    LERR(LERR_PARSING, "expected identifier");
    
    return NULL;
}

static ast_node_t *grammar_fncl(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    if (state->lexems[state->curr_offset    ].type == LEX_IDENTIFIER &&
        state->lexems[state->curr_offset + 1].type == LEX_OPEN_BRACKET &&
        state->lexems[state->curr_offset + 2].type == LEX_CLOSE_BRACKET)
    {
        // to be replaced
        ast_node_t *call_node  = node_pool_claim(pool);
        ast_node_t *ident_node = node_pool_claim(pool);
        if (call_node == NULL || ident_node == NULL)
        {
            node_pool_free(call_node , pool);
            node_pool_free(ident_node, pool);
            return NULL;
        }   

        ident_node->type         = AST_IDENTIFIER;
        ident_node->ident        = state->lexems[state->curr_offset].begin;
        ident_node->ident_length = state->lexems[state->curr_offset].length;

        call_node->type        = AST_OPER_CALL;
        call_node->left_branch = ident_node;

        state->curr_offset += 3;

        return call_node;
    }

    LERR(LERR_PARSING, "expected function call");
    
    return NULL;
}

static ast_node_t *grammar_prim(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    if (state->lexems[state->curr_offset].type == LEX_OPEN_BRACKET)
    {
        size_t old_offset = state->curr_offset;
        state->curr_offset++;

        ast_node_t *expr = grammar_expr(state, pool);
        if (LERR_PRESENT())
        {
            state->curr_offset = old_offset;
            return NULL;
        }
        
        if (state->lexems[state->curr_offset].type != LEX_CLOSE_BRACKET)
        {
            LERR(LERR_PARSING, "expected closing bracket");
            ast_destroy(expr, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
        
        state->curr_offset++;

        return expr;
    }
    
    ast_node_t *fncl = grammar_fncl(state, pool);
    if (fncl != NULL)
        return fncl;
    else
        LERR_RESET();

    ast_node_t *num = grammar_num(state, pool);
    if (num != NULL)
        return num;
    else
        LERR_RESET();
    
    return grammar_var(state, pool);
}

static ast_node_t *grammar_mudi(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = grammar_prim(state, pool);
    if (subtree_root == NULL)
        return NULL;

    while (state->lexems[state->curr_offset].type == LEX_OPERATOR)
    {
        char op = *state->lexems[state->curr_offset].begin;
        if (!(state->lexems[state->curr_offset].length == 1 &&
            (op == '*' || op == '/' || op == '%')))
        {
            break;
        }

        state->curr_offset++;

        ast_node_t *second_arg = grammar_prim(state, pool);
        if (second_arg == NULL)
        {
            ast_destroy(subtree_root, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
            
        ast_node_t *oper_node = node_pool_claim(pool);
        if (oper_node == NULL)
        {
            ast_destroy(subtree_root, pool);
            ast_destroy(second_arg  , pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;

        if (op == '*')
            oper_node->type = AST_OPER_MUL;
        else if (op == '/')
            oper_node->type = AST_OPER_DIV;
        else
            oper_node->type = AST_OPER_MOD;

        subtree_root = oper_node;
    }

    return subtree_root;
}

static ast_node_t *grammar_adsu(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = grammar_mudi(state, pool);
    if (subtree_root == NULL)
        return NULL;

    while (state->lexems[state->curr_offset].type == LEX_OPERATOR)
    {
        char op = *state->lexems[state->curr_offset].begin;
        if (!(state->lexems[state->curr_offset].length == 1 &&
            (op == '+' || op == '-')))
        {
            break;
        }

        state->curr_offset++;

        ast_node_t *second_arg = grammar_mudi(state, pool);
        if (second_arg == NULL)
        {
            ast_destroy(subtree_root, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
            
        ast_node_t *oper_node = node_pool_claim(pool);
        if (oper_node == NULL)
        {
            ast_destroy(subtree_root, pool);
            ast_destroy(second_arg  , pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;

        if (op == '+')
            oper_node->type = AST_OPER_ADD;
        else
            oper_node->type = AST_OPER_SUB;

        subtree_root = oper_node;
    }

    return subtree_root;
}

static ast_node_t *grammar_cmp(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = grammar_adsu(state, pool);
    if (subtree_root == NULL)
        return NULL;

    while (state->lexems[state->curr_offset].type == LEX_OPERATOR)
    {
        const char *op  = state->lexems[state->curr_offset].begin;
        size_t      len = state->lexems[state->curr_offset].length;
        if (!((len == 1 && (strncmp(op,  "<", len) == 0 || strncmp(op,  ">", len) == 0)) ||
            (len == 2 && (strncmp(op, "==", len) == 0 ||  strncmp(op, "!=", len) == 0))))
        {
            break;
        }

        state->curr_offset++;

        ast_node_t *second_arg = grammar_adsu(state, pool);
        if (second_arg == NULL)
        {
            ast_destroy(subtree_root, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
            
        ast_node_t *oper_node = node_pool_claim(pool);
        if (oper_node == NULL)
        {
            ast_destroy(subtree_root, pool);
            ast_destroy(second_arg  , pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        oper_node->left_branch  = subtree_root;
        oper_node->right_branch = second_arg;

        if (strncmp(op, "==", len) == 0)
            oper_node->type = AST_OPER_EQUAL;
        else if (strncmp(op, "!=", len) == 0)
            oper_node->type = AST_OPER_NEQUAL;
        else if (strncmp(op, "<", len) == 0)
            oper_node->type = AST_OPER_LESS;
        else
            oper_node->type = AST_OPER_MORE;

        subtree_root = oper_node;
    }

    return subtree_root;
}

static ast_node_t *grammar_assn(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;

    ast_node_t *var = grammar_var(state, pool);
    if (var == NULL)
        return NULL;

    if (state->lexems[state->curr_offset].type == LEX_OPERATOR)
    {
        char op = *state->lexems[state->curr_offset].begin;
        if (state->lexems[state->curr_offset].length == 1 && op == '=')
        {
            state->curr_offset++;

            ast_node_t *value = grammar_expr(state, pool);
            if (value == NULL)
            {
                ast_destroy(var, pool);
                state->curr_offset = old_offset;
                return NULL;
            }
                
            ast_node_t *assn_node = node_pool_claim(pool);
            if (assn_node == NULL)
            {
                ast_destroy(var, pool);
                ast_destroy(value  , pool);
                state->curr_offset = old_offset;
                return NULL;
            }

            assn_node->type         = AST_OPER_ASSIGN;
            assn_node->left_branch  = var;
            assn_node->right_branch = value;

            return assn_node;
        }
    }

    LERR(LERR_PARSING, "expected = for assigment");
    state->curr_offset = old_offset;
    // destroy var
    return NULL;
}

static ast_node_t *grammar_expr(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    ast_node_t *assn = grammar_assn(state, pool);
    if (assn == NULL)
    {
        LERR_RESET();
        return grammar_cmp(state, pool);
    }

    return assn;
}

static ast_node_t *grammar_expr_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    ast_node_t *expr = grammar_expr(state, pool);
    if (expr == NULL)
        return NULL;

    if (state->lexems[state->curr_offset].type != LEX_SEMICOLON)
    {
        LERR(LERR_PARSING, "expected ;");
        ast_destroy(expr, pool);
        return NULL;
    }

    state->curr_offset++;

    return expr;
}

static ast_node_t *grammar_var_decl_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;
    
    if (state->lexems[state->curr_offset].type != LEX_KW_LET)
    {
        LERR(LERR_PARSING, "expected let keyword");
        return NULL;
    }

    state->curr_offset++;

    if (state->lexems[state->curr_offset].type != LEX_IDENTIFIER)
    {
        LERR(LERR_PARSING, "expected identifier");
        state->curr_offset = old_offset;
        return NULL;
    }

    ast_node_t *ident_node = node_pool_claim(pool);
    if (ident_node == NULL)
    {
        state->curr_offset = old_offset;
        return NULL;
    }

    ident_node->type         = AST_IDENTIFIER;
    ident_node->ident        = state->lexems[state->curr_offset].begin;
    ident_node->ident_length = state->lexems[state->curr_offset].length;

    state->curr_offset++;

    ast_node_t *init_val = NULL;

    if ( state->lexems[state->curr_offset].type == LEX_OPERATOR &&
        *state->lexems[state->curr_offset].begin == '=' &&
         state->lexems[state->curr_offset].length == 1)
    {
        state->curr_offset++;

        init_val = grammar_expr(state, pool);
        if (init_val == NULL)
        {
            ast_destroy(ident_node, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
    }

    if (state->lexems[state->curr_offset].type != LEX_SEMICOLON)
    {
        LERR(LERR_PARSING, "expected ;");
        state->curr_offset = old_offset;
        return NULL;
    }

    state->curr_offset++;

    ast_node_t *decl_node = node_pool_claim(pool);
    if (decl_node == NULL)
    {
        ast_destroy(ident_node, pool);
        ast_destroy(init_val, pool);
        state->curr_offset = old_offset;
        return NULL;
    }

    decl_node->type = AST_VAR_DECL;
    decl_node->left_branch = ident_node;
    decl_node->right_branch = init_val;

    return decl_node;
}

static ast_node_t *grammar_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    ast_node_t *stmt = grammar_comp_stmt(state, pool);
    if (stmt != NULL)
        return stmt;
    else
        LERR_RESET();

    stmt = grammar_var_decl_stmt(state, pool);
    if (stmt != NULL)
        return stmt;
    else
        LERR_RESET();

    return grammar_expr_stmt(state, pool);
}

static ast_node_t *grammar_comp_stmt(parser_state_t *state, node_pool_t *pool)
{
    size_t old_offset = state->curr_offset;
    
    if (state->lexems[state->curr_offset].type != LEX_COMPOUND_BEG)
    {
        LERR(LERR_PARSING, "expected {");
        return NULL;
    }

    state->curr_offset++;

    ast_node_t *subtree_root = NULL;

    while (state->lexems[state->curr_offset].type != LEX_COMPOUND_END)
    {
        ast_node_t *stmt = grammar_stmt(state, pool);
        if (stmt == NULL)
        {
            ast_destroy(subtree_root, pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        if (subtree_root == NULL)
        {
            subtree_root = stmt;
            continue;
        }

        ast_node_t *comp = node_pool_claim(pool);
        if (comp == NULL)
        {
            ast_destroy(subtree_root, pool);
            ast_destroy(stmt, pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        comp->type = AST_COMPOUND;
        comp->left_branch  = subtree_root;
        comp->right_branch = stmt;

        subtree_root = comp;
    }

    state->curr_offset++;

    return subtree_root;
}

static ast_node_t *grammar_fn_decl_stmt(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;
    
    if (state->lexems[state->curr_offset].type != LEX_KW_FN)
    {
        LERR(LERR_PARSING, "expected fn keyword");
        return NULL;
    }

    state->curr_offset++;

    if (state->lexems[state->curr_offset].type != LEX_IDENTIFIER)
    {
        LERR(LERR_PARSING, "expected identifier");
        state->curr_offset = old_offset;
        return NULL;
    }

    ast_node_t *ident_node = node_pool_claim(pool);
    if (ident_node == NULL)
    {
        state->curr_offset = old_offset;
        return NULL;
    }

    ident_node->type         = AST_IDENTIFIER;
    ident_node->ident        = state->lexems[state->curr_offset].begin;
    ident_node->ident_length = state->lexems[state->curr_offset].length;

    state->curr_offset++;

    if (state->lexems[state->curr_offset].type != LEX_OPEN_BRACKET)
    {
        LERR(LERR_PARSING, "expected (");
        state->curr_offset = old_offset;
        ast_destroy(ident_node, pool);
        return NULL;
    }

    state->curr_offset++;

    if (state->lexems[state->curr_offset].type != LEX_CLOSE_BRACKET)
    {
        LERR(LERR_PARSING, "expected )");
        state->curr_offset = old_offset;
        ast_destroy(ident_node, pool);
        return NULL;
    }

    state->curr_offset++;

    ast_node_t *func_body = grammar_comp_stmt(state, pool);
    if (func_body == 0)
    {
        state->curr_offset = old_offset;
        ast_destroy(ident_node, pool);
        return NULL;
    }
    
    ast_node_t *decl_node = node_pool_claim(pool);
    if (decl_node == NULL)
    {
        ast_destroy(ident_node, pool);
        ast_destroy(func_body , pool);
        state->curr_offset = old_offset;
        return NULL;
    }

    decl_node->type         = AST_FUNC_DECL;
    decl_node->left_branch  = ident_node;
    decl_node->right_branch = func_body;

    return decl_node;
}

static ast_node_t *grammar_prg(parser_state_t *state, node_pool_t *pool)
{
    LERR_RESET();

    size_t old_offset = state->curr_offset;

    ast_node_t *subtree_root = NULL;

    while (state->lexems[state->curr_offset].type != LEX_PRG_END)
    {
        ast_node_t *func = grammar_fn_decl_stmt(state, pool);
        if (func == NULL)
        {
            ast_destroy(subtree_root, pool);
            state->curr_offset = old_offset;
            return NULL;
        }
        
        if (subtree_root == NULL)
        {
            subtree_root = func;
            continue;
        }

        ast_node_t *comp = node_pool_claim(pool);
        if (comp == NULL)
        {
            ast_destroy(subtree_root, pool);
            ast_destroy(func, pool);
            state->curr_offset = old_offset;
            return NULL;
        }

        comp->type         = AST_COMPOUND;
        comp->left_branch  = subtree_root;
        comp->right_branch = func;

        subtree_root = comp;
    }

    state->curr_offset++;

    return subtree_root;
}