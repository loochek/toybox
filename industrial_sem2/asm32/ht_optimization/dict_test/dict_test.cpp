#include <clocale>

#include "../dict/dict.hpp"
#include "db.hpp"

#include "benchmark_keys.inl"

const int INIT_BUCKET_COUNT = 2;

int main()
{
    lstatus_t status = LSTATUS_OK;

    printf("Welcome to English-to-Russian dictionary!\nLoading...\n");

    dict_t dict = {0};
    if ((status = dict_construct(&dict, INIT_BUCKET_COUNT)) != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        fprintf(stderr, "Internal error!\n");
        return -1;
    }

    char *db_buffer = nullptr;

    if ((status = db_load("eng_rus_dict.db", &dict, &db_buffer)) != LSTATUS_OK)
    {
        LS_ERR_PRINT();
        fprintf(stderr, "Unable to load database!\n");
        dict_destruct(&dict);
        return -1;
    }

    printf("Write an English word and get Russian equivalent!\n");

#ifndef BENCHMARK
    char key[32] = {0};
    while (true)
    {
        scanf("%31s", key);
        if (strcmp(key, "q") == 0)
            break;

        const char *value = nullptr;
        if (dict_lookup(&dict, key, &value) == LSTATUS_DICT_BAD_KEY)
            printf("Word not found\n");
        else
            printf("%s\n", value);
    }
#else
    const char *value = nullptr;
    for (int i = 0; i < 1000000000; i++)
    {
        dict_lookup(&dict, benchmark_keys[rand() % bench_keys_cnt], &value);
    }

#endif

    dict_destruct(&dict);
    free(db_buffer);
    return 0;
}