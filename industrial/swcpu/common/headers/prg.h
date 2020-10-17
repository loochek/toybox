#include <stdint.h>

typedef struct __attribute__((packed))
{
    char          signature[4];
    uint32_t      format_ver;
    char          program_name[20];
    uint32_t      code_size;
} prg_header_t;

typedef struct
{
    prg_header_t  *prg_header;
    unsigned char *code; 
} program_t;

program_t* load_program_from_file(const char *file_name);
void       program_unload(program_t *prg);