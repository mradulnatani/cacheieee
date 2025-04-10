#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree.h"



TreeNode* create_node(const char* key, const char* value, time_t expiry) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!node) return NULL;

    strncpy(node->key, key, MAX_KEY_SIZE - 1);
    node->key[MAX_KEY_SIZE - 1] = '\0';

    strncpy(node->value, value, MAX_VAL_SIZE - 1);
    node->value[MAX_VAL_SIZE - 1] = '\0';

    node->expiry = expiry;
    node->left = node->right = NULL;
    return node;
}

TreeNode* insert(TreeNode* root, const char* key, const char* value, time_t expiry) {
    if (!root) return create_node(key, value, expiry);

    int cmp = strcmp(key, root->key);
    if (cmp < 0)
        root->left = insert(root->left, key, value, expiry);
    else if (cmp > 0)
        root->right = insert(root->right, key, value, expiry);
    else {
        strncpy(root->value, value, MAX_VAL_SIZE - 1);
        root->value[MAX_VAL_SIZE - 1] = '\0';
        root->expiry = expiry;
    }
    return root;
}

TreeNode* find(TreeNode* root, const char* key) {
    if (!root) return NULL;

    int cmp = strcmp(key, root->key);
    if (cmp == 0) return root;
    if (cmp < 0) return find(root->left, key);
    return find(root->right, key);
}

TreeNode* find_min(TreeNode* node) {
    while (node && node->left) node = node->left;
    return node;
}

TreeNode* delete(TreeNode* root, const char* key) {
    if (!root) return NULL;

    int cmp = strcmp(key, root->key);
    if (cmp < 0)
        root->left = delete(root->left, key);
    else if (cmp > 0)
        root->right = delete(root->right, key);
    else {
        if (!root->left) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (!root->right) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        TreeNode* temp = find_min(root->right);
        strncpy(root->key, temp->key, MAX_KEY_SIZE - 1);
        root->key[MAX_KEY_SIZE - 1] = '\0';
        strncpy(root->value, temp->value, MAX_VAL_SIZE - 1);
        root->value[MAX_VAL_SIZE - 1] = '\0';
        root->expiry = temp->expiry;
        root->right = delete(root->right, temp->key);
    }
    return root;
}

void inorder_keys(TreeNode* root, int sock) {
    if (!root) return;
    inorder_keys(root->left, sock);
    dprintf(sock, "%s\n", root->key);
    inorder_keys(root->right, sock);
}

void clean_expired(TreeNode** root) {
    if (!*root) return;
    clean_expired(&(*root)->left);
    clean_expired(&(*root)->right);
    if ((*root)->expiry > 0 && (*root)->expiry <= time(NULL)) {
        *root = delete(*root, (*root)->key);
    }
}

void free_tree(TreeNode* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
