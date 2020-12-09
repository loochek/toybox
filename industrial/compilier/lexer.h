#ifndef LEXER_H
#define LEXER_H

typedef enum
{
    LEX_KW_FN,
    LEX_KW_IF,
    LEX_KW_LET,
    LEX_OPERATOR,
    LEX_IDENTIFIER,
    LEX_NUMBER,
    LEX_OPEN_BRACKET,
    LEX_CLOSE_BRACKET,
    LEX_COMMA,
    LEX_SEMICOLON,
    LEX_COMPOUND_BEG,
    LEX_COMPOUND_END,
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
 * return -1 if failed
 */
int create_lexical_array(lexem_t **lexems_ptr, const char *src);

#endif