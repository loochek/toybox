#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ncurses.h>

#include "lerror.h"


const size_t MAX_PRG_SIZE = 1000;

/**
 * returns count of bytes read or -1 in case of fail (see lerrno)
 */
ssize_t load_program(const char *file_name, uint8_t **buffer_ptr)
{
    LERR_RESET();

    *buffer_ptr = NULL;

    int fd = open(file_name, O_RDONLY);
    if (fd == -1)
    {
        LERR(LERR_FILE_IO, "Unable to open file for reading: %s", file_name);
        return -1;
    }

    *buffer_ptr = (uint8_t*)calloc(MAX_PRG_SIZE, sizeof(uint8_t));
    if (!(*buffer_ptr))
    {
        LERR(LERR_MEMORY, "Unable to allocate memory");
        return -1;
    }

    ssize_t bytes_read = read(fd, *buffer_ptr, MAX_PRG_SIZE);
    if (bytes_read == -1)
    {
        LERR(LERR_FILE_IO, "Failed to read file");
        free(*buffer_ptr);
        return -1;
    }

    close(fd);
    return bytes_read;
}


/**
 * sets lerrno if failed
 */
void write_program(const char *file_name, const uint8_t *buffer_ptr, size_t bytes_count)
{
    LERR_RESET();

    int fd = open(file_name, O_WRONLY);
    if (fd == -1)
    {
        LERR(LERR_FILE_IO, "Unable to open file for writing: %s", file_name);
        return;
    }

    ssize_t bytes_written = write(fd, buffer_ptr, bytes_count);
    if (bytes_count != bytes_written)
    {
        LERR(LERR_FILE_IO, "Failed to write file");
        return;
    }

    close(fd);
}


const uint8_t  HASH_PARAM_A = 0xB6;
const uint16_t HASH_PARAM_B = 0xD4F6;

uint16_t calculate_hash(uint8_t *buffer, size_t size)
{
    uint16_t hash = 0;

    for (int i = 0; i < size; i++)
    {
        hash += buffer[i] ^ HASH_PARAM_A;
        hash ^= HASH_PARAM_B;
    }

    return hash;
}


void patch(uint8_t *prg_buffer)
{
    // mov ax, 0x118 (password bypass)
    prg_buffer[0x0] = 0xB8;
    prg_buffer[0x1] = 0x18;
    prg_buffer[0x2] = 0x01;
    // jmp ax
    prg_buffer[0x3] = 0xFF;
    prg_buffer[0x4] = 0xE0;
}

void alert(const char *msg)
{
    clear();
    refresh();
    
    int width = strlen(msg) + 2;
    int height = 3;

    int row_offset     = (LINES - height) / 2;
    int collumn_offset = (COLS  - width ) / 2;
    
    WINDOW *msg_box = newwin(height, width, row_offset, collumn_offset);
    box(msg_box, 0, 0);
    mvwprintw(msg_box, 1, 1, msg);
    wrefresh(msg_box);

    delwin(msg_box);
}

void progress()
{
    clear();
    refresh();
    
    int width = 50;
    int height = 7;

    int row_offset     = (LINES - height) / 2;
    int collumn_offset = (COLS  - width ) / 2;
    
    WINDOW *msg_box = newwin(height, width, row_offset, collumn_offset);
    box(msg_box, 0, 0);
    mvwprintw(msg_box, 1, 21, "Patching");
    wrefresh(msg_box);

    for (int i = 0; i < 46; i++)
    {
        mvwaddch(msg_box, 5, 2 + i, '#');
        wrefresh(msg_box);
        usleep(100000);
    }

    delwin(msg_box);
}

const char    *patch_target = "IVAN.COM";
const uint16_t CORRECT_HASH = 0xE099; // IVAN.COM

int main()
{
    uint8_t *prg_buffer = NULL;

    initscr();
    noecho();
    cbreak();
    curs_set(0);

    alert("This program will patch your IVAN.COM. Continue [y/n]?");

    int ch = 0;
    while (ch != 'y' && ch != 'n')
        ch = getch();

    if (ch == 'n')
    {
        endwin();
        return 0;
    }

    ssize_t program_size = load_program(patch_target, &prg_buffer);
    if (program_size == -1)
    {
        alert("Failed to load program! Make sure that IVAN.COM exists!");
        getch();
        goto FAIL;
    }

    uint16_t hash = calculate_hash(prg_buffer, program_size);
    if (hash != CORRECT_HASH)
    {
        alert("Hash mismatch: this patcher requires original IVAN.COM");
        getch();
        goto FAIL;
    }

    progress();

    patch(prg_buffer);

    write_program(patch_target, prg_buffer, program_size);
    if (LERR_PRESENT())
    {
        printf("Failed to write program! Make sure that you have permissions!");
        getch();
        goto FAIL;
    }

    alert("Patched succesfully!");
    getch();

    alert("In memory of xatab...");
    usleep(500000);

    endwin();
    free(prg_buffer);
    return 0;

FAIL:
    free(prg_buffer);
    endwin();
    return -1;
}