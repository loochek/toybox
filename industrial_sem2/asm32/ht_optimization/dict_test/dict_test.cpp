#include <clocale>

#include "../dict/dict.hpp"
#include "db.hpp"

#include "benchmark_keys.inl"

#ifdef VALGRIND
const int BENCHMARK_ITERATIONS = 1000000;
#else
const int BENCHMARK_ITERATIONS = 100000000;
#endif

const int INIT_BUCKET_COUNT = 2;

unsigned int rand_xorshift()
{
    static unsigned int rand_state = 7;

	rand_state ^= rand_state << 13;
	rand_state ^= rand_state >> 17;
	rand_state ^= rand_state << 5;
	return rand_state;
}

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
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++)
    {
        dict_lookup(&dict, benchmark_keys[rand_xorshift() % bench_keys_cnt], &value);
    }

#endif

    dict_destruct(&dict);
    free(db_buffer);
    return 0;
}