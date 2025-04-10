#ifndef TREE_H
#define TREE_H

#include <time.h>

#define MAX_KEY_SIZE 256
#define MAX_VAL_SIZE 1024

typedef struct TreeNode {
    char key[MAX_KEY_SIZE];
    char value[MAX_VAL_SIZE];
    time_t expiry;
    struct TreeNode *left, *right;
} TreeNode;

TreeNode* create_node(const char* key, const char* value, time_t expiry);
TreeNode* insert(TreeNode* root, const char* key, const char* value, time_t expiry);
TreeNode* find(TreeNode* root, const char* key);
TreeNode* delete(TreeNode* root, const char* key);
void inorder_keys(TreeNode* root, int sock);
void clean_expired(TreeNode** root);
void free_tree(TreeNode* root);

#endif
