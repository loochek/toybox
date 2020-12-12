#ifndef LEXER_H
#define LEXER_H

typedef enum
{
    LEX_DUMMY,

    LEX_IDENTIFIER,
    LEX_NUMBER,

    LEX_KW_FN,
    LEX_KW_IF,
    LEX_KW_LET,

    LEX_OPEN_BRACKET,
    LEX_CLOSE_BRACKET,
    LEX_COMMA,
    LEX_SEMICOLON,
    LEX_COMPOUND_BEG,
    LEX_COMPOUND_END,

    LEX_OPER_ADD,
    LEX_OPER_SUB,
    LEX_OPER_MUL,
    LEX_OPER_DIV,
    LEX_OPER_MOD,
    LEX_OPER_ASSIGN,
    LEX_OPER_EQUAL,
    LEX_OPER_NEQUAL,
    LEX_OPER_LESS,
    LEX_OPER_MORE,
    LEX_OPER_ELESS,
    LEX_OPER_EMORE,
    
    LEX_PRG_END
} lexem_type_t;

typedef struct lexem
{
    lexem_type_t type;

    union
    {
        int value;

        struct
        {
            const char *begin;
            size_t length;
        };
    };
} lexem_t;

/**
 * LERR-affecting
 * return NULL if failed
 */
lexem_t *create_lexical_array(const char *src);

#endif