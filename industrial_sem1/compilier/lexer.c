#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "lexer.h"
#include "file_utils.h"
#include "lerror.h"
#include "stack/stack_common.h"

#define TYPE lexem
#define elem_t lexem_t
#include "stack/stack.h"
#undef elem_t
#undef TYPE

typedef struct
{
    lexem_type_t type;
    const char *sample;
} type_sample_t;

/**
 * Lexer templates for special sequences
 */

static const char *COMMENT_SEQ = "//";

static const type_sample_t KEYWORDS[] = 
{
    { LEX_KW_FN    , "fn" },
    { LEX_KW_LET   , "let"},
    { LEX_KW_IF    , "if" },
    { LEX_KW_WHILE , "while" },
    { LEX_KW_RETURN, "return" }
};

// note that the order is important! (= and == for example - firstly test for ==)
static const type_sample_t SIGNS[] = 
{
    { LEX_OPER_EQUAL , "==" },
    { LEX_OPER_NEQUAL, "!=" },
    { LEX_OPER_ELESS , "<=" },
    { LEX_OPER_EMORE , ">=" },
    { LEX_OPER_ADD   , "+"  },
    { LEX_OPER_SUB   , "-"  },
    { LEX_OPER_MUL   , "*"  },
    { LEX_OPER_DIV   , "/"  },
    { LEX_OPER_MOD   , "%"  },
    { LEX_OPER_ASSIGN, "="  },
    { LEX_OPER_LESS  , "<"  },
    { LEX_OPER_MORE  , ">"  },

    { LEX_COMPOUND_BEG , "{"  },
    { LEX_COMPOUND_END , "}"  },
    { LEX_OPEN_BRACKET , "("  },
    { LEX_CLOSE_BRACKET, ")"  },
    { LEX_COMMA        , ","  },
    { LEX_SEMICOLON    , ";"  }
};

static bool is_ident        (char c);
static void skip_non_program(size_t *curr_pos, const char *src);
static bool try_read_number (lexem_t *lexem, size_t *curr_pos, const char *src);
static bool try_read_ident  (lexem_t *lexem, size_t *curr_pos, const char *src);
static bool try_read_sign   (lexem_t *lexem, size_t *curr_pos, const char *src);
static bool try_read_term   (lexem_t *lexem, size_t *curr_pos, const char *src);

void create_lexical_array(const char *src, my_stack_lexem *stack)
{
    LERR_RESET();

    size_t curr_pos = 0;

    bool should_stop = false;

    while (!should_stop)
    {
        skip_non_program(&curr_pos, src);

        lexem_t curr_lexem = {0};

        if      (try_read_number(&curr_lexem, &curr_pos, src)) {}
        else if (try_read_ident (&curr_lexem, &curr_pos, src)) {}
        else if (try_read_sign  (&curr_lexem, &curr_pos, src)) {}
        else if (try_read_term  (&curr_lexem, &curr_pos, src))
        {
            should_stop = true;
        }
        else
        {
            LERR(LERR_LEXING, "unknown symbol sequence at position %zu", curr_pos);
            return;
        }

        STACK_LERR(stack_push_lexem(stack, curr_lexem));
        if (LERR_PRESENT())
            return;
    }
}


static bool is_ident(char c)
{
    return isalpha(c) || isdigit(c) || c == '_';
}

static void skip_non_program(size_t *curr_pos, const char *src)
{
    bool should_stop = false;

    while (!should_stop)
    {
        should_stop = true;

        while (isspace(src[*curr_pos]))
        {
            should_stop = false;
            (*curr_pos)++;
        }

        if (strncmp(&src[*curr_pos], COMMENT_SEQ, strlen(COMMENT_SEQ)) == 0)
        {
            should_stop = false;
            while (src[*curr_pos] != '\n' && src[*curr_pos] != '\0')
                (*curr_pos)++;
        }
    }
}

static bool try_read_number(lexem_t *lexem, size_t *curr_pos, const char *src)
{
    bool success = false;

    *lexem = (lexem_t){0};

    lexem->type  = LEX_NUMBER;
    lexem->value = 0;

    while (isdigit(src[*curr_pos]))
    {
        success = true;

        lexem->value *= 10;
        lexem->value += src[*curr_pos] - '0';
        (*curr_pos)++;
    }

    return success;
}

static bool try_read_ident(lexem_t *lexem, size_t *curr_pos, const char *src)
{
    *lexem = (lexem_t){0};

    lexem->type      = LEX_IDENTIFIER;
    lexem->ident.str = &src[*curr_pos];

    size_t old_pos = *curr_pos;

    while (is_ident(src[*curr_pos]))
        (*curr_pos)++;

    lexem->ident.length = *curr_pos - old_pos;
    if (lexem->ident.length == 0)
        return false;

    for (size_t i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); i++)
    {
        if (strview_c_equ(&lexem->ident, KEYWORDS[i].sample))
        {
            lexem->type = KEYWORDS[i].type;
            break;
        }
    }

    return true;
}

static bool try_read_sign(lexem_t *lexem, size_t *curr_pos, const char *src)
{
    *lexem = (lexem_t){0};

    const char *start_substr = &src[*curr_pos];

    for (size_t i = 0; i < sizeof(SIGNS) / sizeof(SIGNS[0]); i++)
    {
        size_t sample_len = strlen(SIGNS[i].sample);

        if (strncmp(start_substr, SIGNS[i].sample, sample_len) == 0)
        {
            lexem->type = SIGNS[i].type;
            
            *curr_pos += sample_len;
            return true;
        }
    }

    return false;
}

static bool try_read_term(lexem_t *lexem, size_t *curr_pos, const char *src)
{
    *lexem = (lexem_t){0};

    lexem->type = LEX_PRG_END;

    if (src[*curr_pos] != '\0')
        return false;

    (*curr_pos)++;
    return true;
}