#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cdk/cdk.h>

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

const char *COLOR_ERR     = "</02>";
const char *COLOR_DEFAULT = "</05>";

void alert(CDKSCREEN *cdk_screen, char *msg, const char *color)
{
    char *buttons = "OK";
    CDKDIALOG *dialog = newCDKDialog(cdk_screen,
                                     CENTER,
                                     CENTER,
                                     &msg,
                                     1,
                                     &buttons,
                                     1,
                                     A_REVERSE,
                                     TRUE,
                                     TRUE,
                                     TRUE);

    setCDKDialogBackgroundColor(dialog, color);
    activateCDKDialog(dialog, (chtype *)0);

    eraseCDKDialog(dialog);
    destroyCDKDialog(dialog);

    refreshCDKScreen(cdk_screen);
}

#define PROGRESS_BAR_LENGTH       22
#define PROGRESS_BAR_BLOCK_LENGTH 6
#define PROGRESS_TIMER            4
#define PROGRESS_DELAY            50000

void progress(CDKSCREEN *cdk_screen)
{
    char progress_bar[PROGRESS_BAR_LENGTH + 1] = "                      ";
    char *text[2] = { "Patching...",  progress_bar };

    CDKLABEL *label = newCDKLabel(cdk_screen,
                                  CENTER,
                                  CENTER,
                                  text,
                                  2,
                                  TRUE,
                                  TRUE);

    setCDKLabelBackgroundColor(label, COLOR_DEFAULT);

    for (int timer = 0; timer < PROGRESS_TIMER; timer++)
    {
        for (int i = 0; i < PROGRESS_BAR_LENGTH - PROGRESS_BAR_BLOCK_LENGTH; i++)
        {
            progress_bar[i] = ' ';
            progress_bar[i + PROGRESS_BAR_BLOCK_LENGTH] = '#';
            setCDKLabelMessage(label, text, 2);
            activateCDKLabel(label, (chtype *)0);
            usleep(PROGRESS_DELAY);
            eraseCDKLabel(label);
        }

        for (int i = 15; i >= 0; i--)
        {
            progress_bar[i + PROGRESS_BAR_BLOCK_LENGTH] = ' ';
            progress_bar[i] = '#';
            setCDKLabelMessage(label, text, 2);
            activateCDKLabel(label, (chtype *)0);
            usleep(PROGRESS_DELAY);
            eraseCDKLabel(label);
        }
    }

    eraseCDKLabel(label);
    destroyCDKLabel(label);
    refreshCDKScreen(cdk_screen);
}

#define MEMORIAL_DELAY 300000

void memorial(CDKSCREEN *cdk_screen)
{
    char *text = "In memory of xatab...";

    CDKLABEL *label = newCDKLabel(cdk_screen,
                                  CENTER,
                                  CENTER,
                                  &text,
                                  1,
                                  TRUE,
                                  TRUE);

    setCDKLabelBackgroundColor(label, COLOR_DEFAULT);

    activateCDKLabel(label, (chtype *)0);
    usleep(MEMORIAL_DELAY);
    
    eraseCDKLabel(label);
    destroyCDKLabel(label);
    refreshCDKScreen(cdk_screen);
}


int patch_ask(CDKSCREEN *cdk_screen)
{
    char *patch_msg = "This program will patch your IVAN.COM. Continue?";
    char *patch_buttons[2] = { "No", "Yes" };

    CDKDIALOG *patch_dialog = newCDKDialog(cdk_screen,
                                           CENTER,
                                           CENTER,
                                           &patch_msg,
                                           1,
                                           patch_buttons,
                                           2,
                                           A_REVERSE,
                                           TRUE,
                                           TRUE,
                                           TRUE);

    setCDKDialogBackgroundColor(patch_dialog, COLOR_DEFAULT);

    int selection = activateCDKDialog(patch_dialog, (chtype *)0);

    eraseCDKDialog(patch_dialog);
    destroyCDKDialog(patch_dialog);
    refreshCDKScreen(cdk_screen);
    
    return selection;
}

const char    *patch_target  = "IVAN.COM";
const uint16_t CORRECT_HASH  = 0xE099; // IVAN.COM

int main()
{
    uint8_t *prg_buffer = NULL;

    CDKSCREEN *cdk_screen = initCDKScreen(NULL);
    curs_set(0);
    initCDKColor();

    if (!patch_ask(cdk_screen))
    {
        destroyCDKScreen(cdk_screen);
        endCDK();
        return 0;
    }

    ssize_t program_size = load_program(patch_target, &prg_buffer);
    if (program_size == -1)
    {
        alert(cdk_screen, "Failed to load program! Check IVAN.COM existence and permissions!", COLOR_ERR);
        goto FAIL;
    }

    uint16_t hash = calculate_hash(prg_buffer, program_size);
    if (hash != CORRECT_HASH)
    {
        alert(cdk_screen, "Hash mismatch: this patcher requires original IVAN.COM", COLOR_ERR);
        goto FAIL;
    }

    progress(cdk_screen);

    patch(prg_buffer);

    write_program(patch_target, prg_buffer, program_size);
    if (LERR_PRESENT())
    {
        alert(cdk_screen, "Failed to write program! Make sure that you have permissions!", COLOR_ERR);
        goto FAIL;
    }
    free(prg_buffer);

    alert(cdk_screen, "Patched succesfully!", COLOR_DEFAULT);
    memorial(cdk_screen);

    destroyCDKScreen(cdk_screen);
    endCDK();
    return 0;

FAIL:
    free(prg_buffer);
    destroyCDKScreen(cdk_screen);
    endCDK();
    return -1;
}