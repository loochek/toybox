#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "lexer.h"
#include "file_utils.h"
#include "lerror.h"

#define MAX_LEXEM_COUNT 100

// Needs some refactor

typedef enum
{
    STATE_NUMBER,
    STATE_IDENTIFIER,
    STATE_COMMENT,
    STATE_SPEC_SYM,
    STATE_WHITESPACE
} parsing_state_t;

static bool is_ident(char c)
{
    return isalpha(c) || isdigit(c) || c == '_';
}

// some aliases

#define LEX_RESET(STATE)              \
{                                     \
    curr_lexem_length = 0;            \
    curr_lexem        = (lexem_t){0}; \
    curr_state        = STATE;        \
}

#define FINISH_PARSING() goto parsing_finished
#define  PARSING_ERROR() goto parsing_failed

int create_lexical_array(lexem_t **lexems_ptr, const char *src)
{
    LERR_RESET();

    if (lexems_ptr == NULL)
    {
        LERR(LERR_BAD_ARG, "null pointer passed");
        return -1;
    }

    size_t src_length = strlen(src);

    size_t          curr_symbol       = 0;
    size_t          curr_lexem_length = 0;
    lexem_t         curr_lexem        = {0};
    parsing_state_t curr_state        = STATE_NUMBER;

    lexem_t *lexems         = *lexems_ptr;
    size_t   curr_lexem_cnt = 0;

    lexems = calloc(MAX_LEXEM_COUNT, sizeof(lexem_t));
    if (lexems == NULL)
    {
        LERR(LERR_ALLOC, "unable to allocate memory");
        return -1;
    }

    while (true)
    {
        switch (curr_state)
        {
        case STATE_NUMBER:
        {
            if (isdigit(src[curr_symbol]))
            {
                curr_lexem.value = curr_lexem.value * 10 + src[curr_symbol] - '0';
                curr_symbol++;

                curr_lexem_length++;
            }
            else
            {
                if (curr_lexem_length != 0)
                {
                    curr_lexem.type = LEX_NUMBER;
                    lexems[curr_lexem_cnt] = curr_lexem;
                    curr_lexem_cnt++;
                }

                LEX_RESET(STATE_IDENTIFIER);
                curr_lexem.begin = &src[curr_symbol];
            }

            break;
        }

        case STATE_IDENTIFIER:
        {
            if (is_ident(src[curr_symbol]))
            {
                curr_symbol++;
                curr_lexem_length++;
            }
            else
            {
                if (curr_lexem_length != 0)
                {
                    curr_lexem.length = &src[curr_symbol - 1] - curr_lexem.begin + 1;

                    if (strncmp(curr_lexem.begin, "if", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_KW_IF;
                    else if (strncmp(curr_lexem.begin, "fn", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_KW_FN;
                    else if (strncmp(curr_lexem.begin, "let", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_KW_LET;
                    else
                        curr_lexem.type = LEX_IDENTIFIER;

                    lexems[curr_lexem_cnt] = curr_lexem;
                    curr_lexem_cnt++;
                }

                LEX_RESET(STATE_SPEC_SYM);
                curr_lexem.begin  = &src[curr_symbol];
            }

            break;
        }

        case STATE_SPEC_SYM:
        {
            if (!is_ident(src[curr_symbol]) && !isspace(src[curr_symbol]) && src[curr_symbol] != '\0')
            {
                curr_symbol++;
                curr_lexem_length++;

                if (curr_lexem_length == 2 && strncmp(curr_lexem.begin, "//", 2) == 0)
                    LEX_RESET(STATE_COMMENT);
            }
            else
            {
                if (curr_lexem_length != 0)
                {
                    curr_lexem.length = &src[curr_symbol - 1] - curr_lexem.begin + 1;
                    
                    if (strncmp(curr_lexem.begin, "(", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_OPEN_BRACKET;
                    else if (strncmp(curr_lexem.begin, ")", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_CLOSE_BRACKET;
                    else if (strncmp(curr_lexem.begin, "{", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_COMPOUND_BEG;
                    else if (strncmp(curr_lexem.begin, "}", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_COMPOUND_END;
                    else if (strncmp(curr_lexem.begin, ";", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_SEMICOLON;
                    else if (strncmp(curr_lexem.begin, ",", curr_lexem_length) == 0)
                        curr_lexem.type = LEX_COMMA;
                    else
                        curr_lexem.type = LEX_OPERATOR;

                    lexems[curr_lexem_cnt] = curr_lexem;
                    curr_lexem_cnt++;
                }

                if (isspace(src[curr_symbol]))
                {
                    LEX_RESET(STATE_WHITESPACE);
                }
                else if (is_ident(src[curr_symbol]))
                {
                    LEX_RESET(STATE_NUMBER);
                }
                else if (src[curr_symbol] == '\0')
                    FINISH_PARSING();
                else
                {
                    LERR(LERR_LEX_PARSING, "unknown character: %c", src[curr_symbol]);
                    PARSING_ERROR();
                }
            }
            
            break;
        }

        case STATE_WHITESPACE:
        {
            if (isspace(src[curr_symbol]))
                curr_symbol++;
            else
                LEX_RESET(STATE_NUMBER);

            break;
        }

        case STATE_COMMENT:
        {
            if (src[curr_symbol] != '\n')
                curr_symbol++;
            else
                LEX_RESET(STATE_NUMBER);

            break;
        }
        }
    }

parsing_finished:
    lexems[curr_lexem_cnt] = (lexem_t){LEX_PRG_END};
    curr_lexem_cnt++;

    *lexems_ptr = lexems;
    return 0;

parsing_failed:
    free(lexems);
    return -1;
}