#include <stdlib.h>
#include <stdio.h>

#include "../common/headers/lerror.h"
#include "headers/prg.h"

program_t* load_program_from_file(const char *file_name)
{
    FILE          *file       = NULL;
    prg_header_t  *prg_header = NULL;
    unsigned char *code       = NULL;
    program_t     *program    = NULL;

    file = fopen(file_name, "rb");
    if (file == NULL)
    {
        LERR(LERR_FILE_IO, "Unable to open file");
        goto error_handler;
    }

    prg_header = calloc(1, sizeof(prg_header_t));
    if (prg_header == NULL)
        goto malloc_error;
    if (fread(prg_header, sizeof(prg_header_t), 1, file) != 1)
        goto file_error;

    if (!(prg_header->signature[0] == 'L' &&
          prg_header->signature[1] == 'P' &&
          prg_header->signature[2] == 'R' &&
          prg_header->signature[3] == 'G'))
        goto prg_header_error;

    if (prg_header->format_ver != BUILD_NUMBER)
    {
        LERR(LERR_VERSION_MISMATCH, "Version number mismatch");
        goto error_handler;
    }
    
    if (prg_header->program_name[19] != '\0')
        goto prg_header_error;

    code = calloc(prg_header->code_size, sizeof(unsigned char));
    if (code == NULL)
        goto malloc_error;
    if (fread(code, sizeof(unsigned char), prg_header->code_size, file) != prg_header->code_size)
        goto file_error;

    program = calloc(1, sizeof(program_t));
    if (program == NULL)
        goto malloc_error;
    program->prg_header = prg_header;
    program->code = code;

    return program;

file_error:
    LERR(LERR_FILE_IO, "File IO fail (or invalid PRG)");
    goto error_handler;

prg_header_error:
    LERR(LERR_INVALID_PRG, "Invalid PRG header");
    goto error_handler;

malloc_error:
    LERR(LERR_MALLOC, "Unable to allocate memory");
    goto error_handler;
    
error_handler:
    if (file != NULL)
        fclose(file);
    free(prg_header);
    free(code);
    free(program);
    return NULL;
}

void program_unload(program_t *prg)
{
    free(prg->prg_header);
    free(prg->code);
    free(prg);
}