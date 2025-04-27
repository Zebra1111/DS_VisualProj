#include "bsttree.h"

BSTTree::BSTTree() : root(nullptr) {}

BSTTree::~BSTTree() {
    clear();
}

TreeNode* BSTTree::insert(TreeNode *node, int value) {
    if (node == nullptr) {
        return new TreeNode(value);
    }
    if (value < node->value) {
        node->left = insert(node->left, value);
    } else {
        node->right = insert(node->right, value);
    }
    return node;
}

TreeNode* BSTTree::insert(int value) {
    root = insert(root, value);
    return root;
}

TreeNode* BSTTree::getRoot() const {
    return root;
}

TreeNode* BSTTree::find(TreeNode *node, int value) const {
    if (node == nullptr) {
        return nullptr;
    }
    if (node->value == value) {
        return node;
    } else if (value < node->value) {
        return find(node->left, value);
    } else {
        return find(node->right, value);
    }
}

TreeNode* BSTTree::find(int value) const {
    return find(root, value);
}

void BSTTree::clear(TreeNode *node) {
    if (node == nullptr) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

void BSTTree::clear() {
    clear(root);
    root = nullptr;
}

