#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

/*

Река

Основная идея - дерево (в данном случае splay) по неявному ключу

Возьмем splay-дерево с полем value, и пусть в нем ключами будут индексы элементов в массиве.
Заметим, что они несложно вычисляются из размеров поддеревьев - будем их поддерживать. Теперь уберем
ключи - мы можем вычислять их из размеров поддеревьев. При помощи таких неявных ключей мы сможем
реализовать концепцию динамического массива, а именно - резать дерево по принципу "оставить слева n
ключей" и склеивать обратно - то есть, как нам угодно манипулировать с массивом. Неявность ключей
обеспечивает нам гибкость - отрезав часть от дерева, не нужно пересчитывать ключи.

Splay никак не поменяется - там не используются ключи, а повороты, очевидно, ничего не портят.
Аналогично с merge. Поменяется split - там мы спуском находили элемент, который мы поместим в корень
и по которому мы будем резать массив. Соответственно, явный ключ заменим вычислением неявного, иных
изменений нет.

Имея split и merge, мы можем теперь за логарифм всячески резать массивы на подмассивы и склеивать
обратно - что собственно и требуется в задаче. В остальном - просто делаем, что нас просят - вычленяем
предприятия, делим их, отдаем их землю соседям. Изначально мы добавляем n элементов за, соответственно,
O(nlogn). Так как оба запроса в задаче - просто константное количество операций split и merge, 
то обработка k запросов будет работать за O(klogn)

Итоговая асимптотика O((n + k)*logn)

*/

template<typename T>
struct splay_node_t
{
    T                value;
    size_t           tree_size;

    size_t           salary;
    char*            name;

    splay_node_t<T> *parent;
    splay_node_t<T> *left_child;
    splay_node_t<T> *right_child;

    splay_node_t() = delete;
    splay_node_t(const T &value, size_t salary, char* name) : value(value), salary(salary),
                name(name), tree_size(1), parent(nullptr), left_child(nullptr), right_child(nullptr) {}
};

template<typename T>
splay_node_t<T> *splay_get_left_child(const splay_node_t<T> *node)
{
    return node == nullptr ? nullptr : node->left_child;
}

template<typename T>
splay_node_t<T> *splay_get_right_child(const splay_node_t<T> *node)
{
    return node == nullptr ? nullptr : node->right_child;
}

template<typename T>
void splay_set_parent(splay_node_t<T> *node, splay_node_t<T> *parent)
{
    if (node == nullptr)
        return;

    node->parent = parent;
}

template<typename T>
void splay_replace_child(splay_node_t<T> *old_child, splay_node_t<T> *new_child)
{
    if (old_child == nullptr)
        return;

    if (old_child->parent == nullptr)
        return;

    if (old_child == old_child->parent->left_child)
        old_child->parent->left_child = new_child;
    
    if (old_child == old_child->parent->right_child)
        old_child->parent->right_child = new_child;
}

template<typename T>
size_t splay_get_tree_size(splay_node_t<T> *node)
{
    return node == nullptr ? 0 : node->tree_size;
}

// in this function you should recalculate your children-dependent node fields
template<typename T>
void splay_recalc_from_children(splay_node_t<T> *node)
{
    if (node == nullptr)
        return;

    node->tree_size = 1 + splay_get_tree_size(node->left_child)
                        + splay_get_tree_size(node->right_child);
}

template<typename T>
splay_node_t<T> *splay_rotate_left(splay_node_t<T> *node)
{
    if (node == nullptr)
        return node;
    if (node->right_child == nullptr)
        return node;

    splay_node_t<T> *new_root = node->right_child;
    node->right_child = new_root->left_child;
    new_root->left_child = node;

    splay_set_parent(node->right_child, node);
    splay_set_parent(new_root, node->parent);
    splay_replace_child(node, new_root);
    splay_set_parent(node, new_root);

    splay_recalc_from_children(node);
    splay_recalc_from_children(new_root);
    
    return new_root;
}

template<typename T>
splay_node_t<T> *splay_rotate_right(splay_node_t<T> *node)
{
    if (node == nullptr)
        return node;
    if (node->left_child == nullptr)
        return node;

    splay_node_t<T> *new_root = node->left_child;
    node->left_child = new_root->right_child;
    new_root->right_child = node;

    splay_set_parent(node->left_child, node);
    splay_set_parent(new_root, node->parent);
    splay_replace_child(node, new_root);
    splay_set_parent(node, new_root);

    splay_recalc_from_children(node);
    splay_recalc_from_children(new_root);

    return new_root;
}

template<typename T>
void splay(splay_node_t<T> *node)
{
    if (node == nullptr)
        return;

    while (node->parent != nullptr)
    {
        if (node == node->parent->left_child)
        {
            // zig
            if (node->parent->parent == nullptr)
                splay_rotate_right(node->parent);
            // zig-zig
            else if (node->parent == node->parent->parent->left_child)
            {
                splay_rotate_right(node->parent->parent);
                splay_rotate_right(node->parent);
            }
            // zig-zag
            else if (node->parent == node->parent->parent->right_child)
            {
                splay_rotate_right(node->parent);
                splay_rotate_left (node->parent->parent);
            }
        }
        else if (node == node->parent->right_child)
        {
            // zig
            if (node->parent->parent == nullptr)
                splay_rotate_left(node->parent);
            // zig-zig
            else if (node->parent == node->parent->parent->right_child)
            {
                splay_rotate_left(node->parent->parent);
                splay_rotate_left(node->parent);
            }
            // zig-zag
            else if (node->parent == node->parent->parent->left_child)
            {
                splay_rotate_left (node->parent);
                splay_rotate_right(node->parent->parent);
            }
        }
    }
}

template<typename T>
splay_node_t<T> *splay_merge(splay_node_t<T> *tree1, splay_node_t<T> *tree2)
{
    if (tree1 == nullptr)
        return tree2;

    if (tree2 == nullptr)
        return tree1;

    splay_node_t<T> *max_node = tree1;
    while (max_node->right_child != nullptr)
        max_node = max_node->right_child;

    splay(max_node);

    max_node->right_child = tree2;
    splay_set_parent(tree2, max_node);
    splay_recalc_from_children(max_node);

    return max_node;
}

template<typename T>
void splay_split(splay_node_t<T> *src_tree, size_t split_cnt,
                    splay_node_t<T> **tree1, splay_node_t<T> **tree2)
{
    if (src_tree == nullptr)
    {
        *tree1 = nullptr;
        *tree2 = nullptr;
        return;
    }

    splay_node_t<T> *to_splay = src_tree;
    size_t implcicit_key = splay_get_tree_size(src_tree->left_child);
    while (true)
    {
        if (split_cnt > implcicit_key)
        {
            if (to_splay->right_child == nullptr)
                break;

            to_splay = to_splay->right_child;
            implcicit_key += 1 + splay_get_tree_size(to_splay->left_child);
        }
        else if (split_cnt < implcicit_key)
        {
            if (to_splay->left_child == nullptr)
                break;
            
            to_splay = to_splay->left_child;
            implcicit_key -= 1 + splay_get_tree_size(to_splay->right_child);
        }
        else
            break;
    }
    splay(to_splay);

    if (splay_get_tree_size(to_splay->left_child) < split_cnt)
    {
        *tree1 = to_splay;
        *tree2 = to_splay->right_child;

        (*tree1)->right_child = nullptr;
        splay_recalc_from_children(*tree1);
        splay_set_parent(*tree2, (splay_node_t<T>*)nullptr);
    }
    else
    {
        *tree1 = to_splay->left_child;
        *tree2 = to_splay;

        (*tree2)->left_child = nullptr;
        splay_recalc_from_children(*tree2);
        splay_set_parent(*tree1, (splay_node_t<T>*)nullptr);
    }
}

template<typename T>
void splay_destruct(splay_node_t<T> *node)
{
    if (node == nullptr)
        return;
    
    splay_destruct(node->left_child);
    splay_destruct(node->right_child);

    free(node->name);
    delete node;
}

#define MAX_NAME_LENGTH 30

int main()
{
    splay_node_t<size_t> *tree_root = nullptr;

    size_t n = 0, m = 0;
    scanf("%zu %zu", &n, &m);

    for (size_t i = 0; i < n; i++)
    {
        char* name = (char*)calloc(MAX_NAME_LENGTH + 1, sizeof(char));
        size_t salary = 0;
        scanf("%s %zu", name, &salary);

        splay_node_t<size_t> *new_node = new splay_node_t<size_t>(0, salary, name);
        tree_root = splay_merge(tree_root, new_node);
    }

    for (size_t i = 0; i < m; i++)
    {
        size_t cmd = 0;
        scanf("%zu", &cmd);

        if (cmd == 0)
        {
            char* name = (char*)calloc(MAX_NAME_LENGTH + 1, sizeof(char));
            size_t salary = 0, pos = 0;
            scanf("%zu %s %zu", &pos, name, &salary);

            splay_node_t<size_t> *new_employer = new splay_node_t<size_t>(0, salary, name);

            splay_node_t<size_t> *left = NULL, *right = NULL;
            splay_split(tree_root, pos - 1, &left, &right);

            tree_root = splay_merge(splay_merge(left, new_employer), right);
        }
        else if (cmd == 1)
        {
            size_t pos = 0;
            scanf("%zu", &pos);

            splay_node_t<size_t> *left = NULL, *right = NULL, *to_fire = NULL;
            splay_split(tree_root, pos - 1, &left, &right);
            splay_split(right, 1, &to_fire, &right);

            splay_destruct(to_fire);

            tree_root = splay_merge(left, right);
        }
        else if (cmd == 2)
        {
            size_t pos = 0;
            scanf("%zu", &pos);

            splay_node_t<size_t> *left = NULL, *right = NULL, *to_show = NULL;
            splay_split(tree_root, pos - 1, &left, &right);
            splay_split(right, 1, &to_show, &right);

            printf("%s %zu\n", to_show->name, to_show->salary);

            tree_root = splay_merge(splay_merge(left, to_show), right);
        }
    }

    //splay_destruct(tree_root);
    return 0;
}