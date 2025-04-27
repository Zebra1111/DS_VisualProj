#ifndef AVLTREE_H
#define AVLTREE_H

#include "binarytree.h"
#include <QDebug>

class AVLTree {
public:
    AVLTree();
    ~AVLTree();

    void insert(int key);
    void remove(int key);
    void clear();
    void clear(TreeNode* node);
    void updateHeight(TreeNode* node);
    int max(int a, int b);
    int height(TreeNode* node);
    int getBalance(TreeNode* node);

    TreeNode* insert(TreeNode* node, int key);
    TreeNode* remove(TreeNode* node, int key);
    TreeNode* minValueNode(TreeNode* node);
    TreeNode* getRoot() const;
    TreeNode* rotateLeft(TreeNode* x);
    TreeNode* rotateRight(TreeNode* y);

private:
    TreeNode* root;
};


#endif // AVLTREE_H
