#include <cctype>

#include "lexer.hpp"

//====================================================

/**
 * Lexer templates
 */

struct lexer_template_t
{
    lexem_type_t type;
    const char *sample;
};

static const lexer_template_t KEYWORDS[] = 
{
    { LEX_KW_FN    , "fn"     },
    { LEX_KW_LET   , "let"    },
    { LEX_KW_IF    , "if"     },
    { LEX_KW_ELSE  , "else"   },
    { LEX_KW_WHILE , "while"  },
    { LEX_KW_RETURN, "return" }
};

/**
 * Note that the lexer tests these sequences sequentially
 * For example, == is tested before =
 */
static const lexer_template_t SPEC_SEQ[] =
{
    { LEX_EQUAL , "==" },
    { LEX_NEQUAL, "!=" },
    { LEX_ELESS , "<=" },
    { LEX_EMORE , ">=" },
    { LEX_ADD   , "+"  },
    { LEX_SUB   , "-"  },
    { LEX_MUL   , "*"  },
    { LEX_DIV   , "/"  },
    { LEX_MOD   , "%"  },
    { LEX_ASSIGN, "="  },
    { LEX_LESS  , "<"  },
    { LEX_MORE  , ">"  },

    { LEX_COMPOUND_BEG , "{"  },
    { LEX_COMPOUND_END , "}"  },
    { LEX_OPEN_BRACKET , "("  },
    { LEX_CLOSE_BRACKET, ")"  },
    { LEX_COMMA        , ","  },
    { LEX_SEMICOLON    , ";"  }
};

static const char *COMMENT_SEQ = "//";

//====================================================

struct lexer_state_t
{
    const char *src;
    int curr_pos;
    int curr_row;
    int curr_col;
};

static bool is_ident(char c);

static void skip_non_program (lexer_state_t *state);
static bool try_read_number  (lexem_t *lexem, lexer_state_t *state);
static bool try_read_ident   (lexem_t *lexem, lexer_state_t *state);
static bool try_read_spec_seq(lexem_t *lexem, lexer_state_t *state);
static bool try_read_term    (lexem_t *lexem, lexer_state_t *state);

#define LEXER_ERROR(err_str, ...) \
    COMPILATION_ERROR(comp_err, state.curr_row, state.curr_col, err_str, ##__VA_ARGS__)

lstatus_t lexer_tokenize(const char *src, list_t<lexem_t> *lexems, compilation_error_t *comp_err)
{
    lstatus_t status = LSTATUS_OK;

    lexer_state_t state = { src, 0, 1, 1 };
    bool should_stop = false;

    while (!should_stop)
    {
        skip_non_program(&state);

        lexem_t curr_lexem = { LEX_DUMMY };

        if      (try_read_number  (&curr_lexem, &state)) {}
        else if (try_read_ident   (&curr_lexem, &state)) {}
        else if (try_read_spec_seq(&curr_lexem, &state)) {}
        else if (try_read_term    (&curr_lexem, &state))
        {
            should_stop = true;
        }
        else
        {
            LEXER_ERROR("unknown symbol sequence");
            return LSTATUS_LEXER_FAIL;
        }

        LSCHK(list_push_front(lexems, curr_lexem));
    }

    return LSTATUS_OK;
}

static bool is_ident(char c)
{
    return isalpha(c) || isdigit(c) || c == '_';
}

static void skip_non_program(lexer_state_t *state)
{
    bool should_stop = false;

    while (!should_stop)
    {
        should_stop = true;

        while (isspace(state->src[state->curr_pos]))
        {
            should_stop = false;

            if (state->src[state->curr_pos] == '\n')
            {
                state->curr_row++;
                state->curr_col = 1;
            }
            else
                state->curr_col++;
            
            state->curr_pos++;
        }

        if (strncmp(&state->src[state->curr_pos], COMMENT_SEQ, strlen(COMMENT_SEQ)) == 0)
        {
            should_stop = false;

            while (state->src[state->curr_pos] != '\n' && state->src[state->curr_pos] != '\0')
                state->curr_pos++;
        }
    }
}

#define INIT_LEXEM(type) *lexem = (lexem_t){ type, state->curr_row, state->curr_col }

static bool try_read_number(lexem_t *lexem, lexer_state_t *state)
{
    INIT_LEXEM(LEX_NUMBER);
    bool success = false;

    while (isdigit(state->src[state->curr_pos]))
    {
        success = true;

        lexem->number *= 10;
        lexem->number += state->src[state->curr_pos] - '0';
        state->curr_pos++;
        state->curr_col++;
    }

    return success;
}

static bool try_read_ident(lexem_t *lexem, lexer_state_t *state)
{
    INIT_LEXEM(LEX_IDENTIFIER);
    lexem->ident.str = &state->src[state->curr_pos];

    int old_pos = state->curr_pos;

    while (is_ident(state->src[state->curr_pos]))
    {
        state->curr_pos++;
        state->curr_col++;
    }

    lexem->ident.length = state->curr_pos - old_pos;
    if (lexem->ident.length == 0)
        return false;

    for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); i++)
    {
        if (strview_c_equ(&lexem->ident, KEYWORDS[i].sample))
        {
            lexem->type = KEYWORDS[i].type;
            break;
        }
    }

    return true;
}

static bool try_read_spec_seq(lexem_t *lexem, lexer_state_t *state)
{
    INIT_LEXEM(LEX_DUMMY);

    for (size_t i = 0; i < sizeof(SPEC_SEQ) / sizeof(SPEC_SEQ[0]); i++)
    {
        int sample_len = strlen(SPEC_SEQ[i].sample);

        if (strncmp(&state->src[state->curr_pos], SPEC_SEQ[i].sample, sample_len) == 0)
        {
            lexem->type = SPEC_SEQ[i].type;
            
            state->curr_pos += sample_len;
            state->curr_col += sample_len;
            return true;
        }
    }

    return false;
}

static bool try_read_term(lexem_t *lexem, lexer_state_t *state)
{
    INIT_LEXEM(LEX_PRG_END);

    if (state->src[state->curr_pos] != '\0')
        return false;

    state->curr_pos++;
    state->curr_col++;
    return true;
}