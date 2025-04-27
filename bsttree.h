#ifndef BSTTREE_H
#define BSTTREE_H

#include "binarytree.h"

class BSTTree {
public:
    BSTTree();
    ~BSTTree();

    TreeNode* insert(int value);
    TreeNode* getRoot() const;
    TreeNode* find(int value) const;
    TreeNode* insert(TreeNode *node, int value);
    TreeNode* find(TreeNode *node, int value) const;
    void clear(TreeNode *node);
    void clear();

private:
    TreeNode *root;

};

#endif // BSTTREE_H
