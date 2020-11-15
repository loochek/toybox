#define MAX_NODE_NAME_LENGTH 100

typedef struct tree_node_t
{
    char node_name[MAX_NODE_NAME_LENGTH + 1];
    
    struct tree_node_t *yes_branch;
    struct tree_node_t *no_branch;
} tree_node_t;

// Throws LERR
tree_node_t *tree_create_from_file(const char *file_name);

void tree_destruct(tree_node_t *tree_root);

// Throws LERR
void tree_dump(tree_node_t *tree_root, const char *file_name);