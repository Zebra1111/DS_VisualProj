#include "avltree.h"
#include <cstdlib>

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    clear(root);
}

void AVLTree::insert(int key) {
    root = insert(root, key);
}

void AVLTree::remove(int key) {
    root = remove(root, key);
}

void AVLTree::clear() {
    clear(root);
    root = nullptr;
}

TreeNode* AVLTree::getRoot() const
{
    return root;
}


TreeNode* AVLTree::insert(TreeNode* node, int key) {
    if (!node) {
        return new TreeNode(key);
    }

    if (key < node->value)
        node->left = insert(node->left, key);
    else if (key > node->value)
        node->right = insert(node->right, key);
    else
        return node; // 不允许重复

    updateHeight(node);
    int balance = getBalance(node);

    if (balance > 1 && key < node->left->value)
        return rotateRight(node); // LL
    if (balance < -1 && key > node->right->value)
        return rotateLeft(node);  // RR
    if (balance > 1 && key > node->left->value) {    // LR
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && key < node->right->value) {  // RL
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

TreeNode* AVLTree::remove(TreeNode* node, int key) {
    if (!node) return nullptr;

    if (key < node->value)
        node->left = remove(node->left, key);
    else if (key > node->value)
        node->right = remove(node->right, key);
    else {
        if (!node->left || !node->right) {
            TreeNode* temp = node->left ? node->left : node->right;
            delete node;
            return temp;
        } else {
            TreeNode* temp = minValueNode(node->right);
            node->value = temp->value;
            node->right = remove(node->right, temp->value);
        }
    }

    if (!node) return node; // 防止空指针

    updateHeight(node);
    int balance = getBalance(node);

    if (balance > 1) {
        if (getBalance(node->left) >= 0)
            return rotateRight(node); // LL
        else {
            node->left = rotateLeft(node->left); // LR
            return rotateRight(node);
        }
    }

    if (balance < -1) {
        if (getBalance(node->right) <= 0)
            return rotateLeft(node); // RR
        else {
            node->right = rotateRight(node->right); // RL
            return rotateLeft(node);
        }
    }
    return node;
}


TreeNode* AVLTree::minValueNode(TreeNode* node) {
    TreeNode* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

int AVLTree::height(TreeNode* node) {
    return node ? node->height : 0;
}

int AVLTree::getBalance(TreeNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

void AVLTree::updateHeight(TreeNode* node) {
    node->height = 1 + max(height(node->left), height(node->right));
}

int AVLTree::max(int a, int b) {
    return (a > b) ? a : b;
}

TreeNode* AVLTree::rotateLeft(TreeNode* x) {
    TreeNode* y = x->right;
    TreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

TreeNode* AVLTree::rotateRight(TreeNode* y) {
    TreeNode* x = y->left;
    TreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

void AVLTree::clear(TreeNode* node) {
    if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

