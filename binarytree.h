#ifndef BINARYTREE_H
#define BINARYTREE_H

struct TreeNode {
    int value;
    int height;
    TreeNode *left, *right;
    TreeNode(int val) : value(val), height(1), left(nullptr), right(nullptr) {}
};

#endif // BINARYTREE_H
