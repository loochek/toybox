#include "dict/dict.hpp"

int main()
{
    dict_t dict = {0};
    dict_construct(&dict, 100);

    dict_insert(&dict, "key1", "value1");
    dict_insert(&dict, "key2", "value2");
    dict_insert(&dict, "key3", "value3");

    dict_destruct(&dict);
    return 0;
}