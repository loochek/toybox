#include <cstdio>

const unsigned long long MAX_NUM_LOG = 40;

struct TrieNode
{
    TrieNode() : zeroNode(nullptr), oneNode(nullptr), terminal(false) , termNum(-1) {};

    TrieNode *zeroNode;
    TrieNode *oneNode;

    bool terminal;
    unsigned long long termNum;
};

void trieInsert(TrieNode *trie, const char *str, unsigned long long num)
{
    while (*str != '\0')
    {
        if (*str == '0')
        {
            if (trie->zeroNode == nullptr)
                trie->zeroNode = new TrieNode();

            trie = trie->zeroNode;
        }
        else
        {
            if (trie->oneNode == nullptr)
                trie->oneNode = new TrieNode();

            trie = trie->oneNode;
        }

        str++;
    }

    trie->terminal = true;
    trie->termNum = num;
}

bool trieRemove(TrieNode *trie, const char *str)
{
    if (*str == '\0')
    {
        if (!trie->terminal)
            return false;

        if (trie->zeroNode == nullptr && trie->oneNode == nullptr)
            return true;
        else
        {
            trie->terminal = false;
            return false;
        }
    }

    if (*str == '0')
    {
        if (trieRemove(trie->zeroNode, str + 1))
        {
            delete trie->zeroNode;
            trie->zeroNode = nullptr;
        }
    }
    else
    {
        if (trieRemove(trie->oneNode, str + 1))
        {
            delete trie->oneNode;
            trie->oneNode = nullptr;
        }
    }

    if (trie->zeroNode == nullptr && trie->oneNode == nullptr && !trie->terminal)
        return true;
    else
        return false;
}

unsigned long long trieSearch(TrieNode *trie, const char *str)
{
    unsigned long long bestNum = -1;
    while (true)
    {
        if (*str == '0')
        {
            if (trie->zeroNode != nullptr)
                trie = trie->zeroNode;
            else if (trie->oneNode != nullptr)
                trie = trie->oneNode;
            else
                break;
        }
        else
        {
            if (trie->oneNode != nullptr)
                trie = trie->oneNode;
            else if (trie->zeroNode != nullptr)
                trie = trie->zeroNode;
            else
                break;
        }

        if (trie->terminal)
            bestNum = trie->termNum;

        str++;
    }

    return bestNum;
}

void trieFree(TrieNode *trie)
{
    if (trie == nullptr)
        return;

    if (trie->oneNode != nullptr)
        trieFree(trie->oneNode);

    if (trie->zeroNode != nullptr)
        trieFree(trie->zeroNode);
    
    delete trie;
}

int main()
{
    unsigned long long n = 0;
    scanf("%llu", &n);

    char buf[MAX_NUM_LOG + 1] = {0};

    TrieNode *trie = new TrieNode();

    for (unsigned long long i = 0; i < n; i++)
    {
        unsigned long long cmd = 0, num = 0;
        scanf("%llu %llu", &cmd, &num);

        char *ptr = buf + MAX_NUM_LOG;
        unsigned long long currNum = num;

        while (ptr != buf)
        {
            ptr--;

            if (currNum & 1)
                *ptr = '1';
            else
                *ptr = '0';

            currNum >>= 1;
        }

        if (cmd == 1)
            trieInsert(trie, ptr, num);
        else if (cmd == 2)
            trieRemove(trie, ptr);
        else if (cmd == 3)
        {
            char *oldPtr = ptr;

            while (*ptr != '\0')
            {
                *ptr = '0' + 1 - (*ptr - '0');
                ptr++;
            }

            printf("%llu\n", trieSearch(trie, oldPtr));
        }
    }

    trieFree(trie);
    return 0;
}