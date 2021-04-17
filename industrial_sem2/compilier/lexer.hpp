#ifndef LEXER_HPP
#define LEXER_HPP

#include "utils/lstatus.hpp"
#include "utils/list.hpp"
#include "utils/string_view.hpp"
#include "compilier_status.hpp"

/**
 * Types of lexems emitted by a lexer
 */
enum lexem_type_t
{
    LEX_DUMMY,

    LEX_IDENTIFIER,
    LEX_NUMBER,

    LEX_KW_FN,
    LEX_KW_IF,
    LEX_KW_WHILE,
    LEX_KW_LET,
    LEX_KW_RETURN,

    LEX_OPEN_BRACKET,
    LEX_CLOSE_BRACKET,
    LEX_COMMA,
    LEX_SEMICOLON,
    LEX_COMPOUND_BEG,
    LEX_COMPOUND_END,

    LEX_ADD,
    LEX_SUB,
    LEX_MUL,
    LEX_DIV,
    LEX_MOD,
    LEX_ASSIGN,
    LEX_EQUAL,
    LEX_NEQUAL,
    LEX_LESS,
    LEX_MORE,
    LEX_ELESS,
    LEX_EMORE,
    
    LEX_PRG_END
};

struct lexem_t
{
    lexem_type_t type;
    int row;
    int collumn;

    union
    {
        int value;
        string_view_t ident;
    };
};

/**
 * Tries to split the given source code into lexems
 * 
 * \param \c src Source code
 * \param \c lexems Where to put emitted lexems
 * \param \c comp_status Compilier status structure
 */
lstatus_t parse_source(const char *src, list_t<lexem_t> *lexems, compilier_status_t *comp_status);

#endif