#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "custom_strcmp.h"

// Будем хранить указатели на начало и конец строк (для перевернутого компаратора) - их и будем сортировать
typedef struct
{
    char *begin;
    char *end;
}  string_entry_t;

// компаратор строк
int comp(const void *f, const void *s)
{
    string_entry_t a = *(string_entry_t*)f;
    string_entry_t b = *(string_entry_t*)s;
    return custom_strcmp(a.begin, b.begin, 0);
}

// перевернутый компаратор
int comp_rev(const void *f, const void *s)
{
    string_entry_t a = *(string_entry_t*)f;
    string_entry_t b = *(string_entry_t*)s;
    // т.к. .end указывает на нуль-терминатор
    return custom_strcmp(a.end - 1, b.end - 1, 1);
}

int get_file_size(FILE *fd)
{
    // определяем размер файла

    if (fseek(fd, 0, SEEK_END) != 0)
        return -1;

    int file_size = ftell(fd);
    if (file_size == -1)
        return -1;
    if (fseek(fd, 0, SEEK_SET) != 0)
        return -1;

    return file_size;
}

// читает строки
// требуется указать адреса buffer, str_cnt, index - в случае успешного чтения
// в них будут лежать указатель на буфер памяти, количество считанных строк 
// и указатель на индекс, в котором для всех строк последовательно хранятся указатели
// на начало и конец строк
// чтение успешно, если возвращен 0, иначе ошибка
// если чтение успешно, вы обязаны освободить память buffer и index!!!
int read_input(char **buffer_ret, size_t *str_cnt, string_entry_t **index_ret)
{
    FILE *fd = fopen("onegin.txt", "rb");
    if (fd == NULL)
        return -1;

    int file_size = -1;
    if ((file_size = get_file_size(fd)) == -1)
        return -1;

    // будем хранить строки последовательно в одном буфере
    // в худшем случае нам нужно sizeof(char) * (file_size + 2) памяти
    // память для \0 обеспечена \n'ами, для последней строки добавим если его нет
    // (по-хорошему в конце файла должен быть \n, но не всегда)
    // еще первым символом запихнем \0, тогда можем бежать по строке и в обратную сторону
    // итого +2
    char* buf = malloc(sizeof(char) * (file_size + 2));
    if (buf == NULL)
    {
        fclose(fd);
        return -1;
    }
    // "вернем" его, свою копию будем двигать
    *buffer_ret = buf;
    // тот самый \0
    *(buf++) = '\0';

    // читаем файл напрямую в буфер
    if (fread(buf, sizeof(char), file_size, fd) != file_size)
    {
        free(*buffer_ret);
        fclose(fd);
        return -1;
    }
    fclose(fd);

    // тот самый \n в конце (проще добавить тут и не делать доп. проверок в дальнейшем)
    if (buf[file_size - 1] != '\n')
    {
        buf[file_size] = '\n';
        // как будто он и был в файле
        file_size++;
    }    
    
    // заменяем \n на \0, параллельно считаем число строк
    // (сколько \n - столько и строк по определению)
    *str_cnt = 0;
    for (size_t i = 0; i < file_size; i++)
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            (*str_cnt)++;
        }
    }
    
    // выделяем память под индекс
    string_entry_t *index = malloc(*str_cnt * sizeof(string_entry_t));
    if (index == NULL)
    {
        free(*buffer_ret);
        fclose(fd);
        return -1;
    }
    *index_ret = index;

    // строим индекс, все достаточно прозрачно
    size_t idx_cnt = 0;
    // первый и последний символы - частные случаи
    index[idx_cnt].begin = buf;
    for (size_t i = 0; i < file_size - 1; i++)
    {
        if (buf[i] == '\0')
        {
            index[idx_cnt].end = buf + i;
            idx_cnt++;
            index[idx_cnt].begin = buf + i + 1;
        }
    }
    index[idx_cnt].end = buf + file_size - 1;
    return 0;
}

// пишет строки из strings в файл fd
int write_output(FILE *fdout, string_entry_t *index, size_t str_cnt)
{
    if (fdout == NULL)
        return -1;
    // выводим первые str_cnt строк из index
    for (size_t i = 0; i < str_cnt; i++)
    {
        if (fputs(index[i].begin, fdout) == EOF)
            return -1;
        if (fputc('\n', fdout) == EOF)
            return -1;
    }
    // перевод строки в конце
    if (fputc('\n', fdout) == EOF)
        return -1;
    return 0;
}

int onegin()
{
    // читаем файл
    char *buffer = NULL;
    string_entry_t *index = NULL;
    size_t str_cnt = 0;
    if (read_input(&buffer, &str_cnt, &index) != 0)
        return -1;
    // скопируем индекс, чтобы сохранить его изначальное состояние
    string_entry_t *index_orig = malloc(str_cnt * sizeof(string_entry_t));
    if (index_orig == NULL)
        return -1;
    memcpy(index_orig, index, str_cnt * sizeof(string_entry_t));

    FILE *fdout = fopen("onegin_parsed.txt", "w");
    if (fdout == NULL)
        return -1;
    // сортируем и выводим
    // обычная сортировка
    qsort(index, str_cnt, sizeof(string_entry_t), comp);
    if (write_output(fdout, index, str_cnt) != 0)
        return -1;
    // перевернутая
    qsort(index, str_cnt, sizeof(string_entry_t), comp_rev);
    if (write_output(fdout, index, str_cnt) != 0)
        return -1;
    // оригинал текста
    if (write_output(fdout, index_orig, str_cnt) != 0)
        return -1;
    fclose(fdout);
    // освобождаем память
    free(buffer);
    free(index);
    free(index_orig);
    return 0;
}

#ifndef TEST
int main(const int argc, const char* argv[])
#else
int dummy()
#endif
{
    if (onegin() != 0)
    {
        printf("An error has occured\n");
        return -1;
    }
    return 0;
}