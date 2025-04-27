#ifndef BTREE_H
#define BTREE_H

#include <QObject>
#include <iostream>

#define DEGREE 2  // 最小度数

class BTreeNode {
public:
    BTreeNode(bool isLeaf);
    ~BTreeNode();

    int keys[2 * DEGREE - 1];         // 关键字数组
    BTreeNode* children[2 * DEGREE];  // 子节点数组
    int numKeys;                      // 当前关键字数
    bool isLeaf;                      // 是否为叶子

    void insertNonFull(int key);               // 插入（非满）
    void splitChild(int i, BTreeNode* y);      // 分裂子节点
    BTreeNode* search(int key);                // 查找关键字

    void remove(int key);                      // 删除关键字
    void removeFromLeaf(int idx);              // 从叶子删除
    void removeFromNonLeaf(int idx);           // 从非叶删除
    int getPred(int idx);                      // 获取前驱
    int getSucc(int idx);                      // 获取后继
    void fill(int idx);                        // 填充子节点
    void borrowFromPrev(int idx);              // 向左兄弟借
    void borrowFromNext(int idx);              // 向右兄弟借
    void merge(int idx);                       // 合并子节点
};

class BTree {
public:
    BTree();
    ~BTree();

    void insert(int key);                      // 插入关键字
    void remove(int key);                      // 删除关键字
    BTreeNode* search(int key);                // 查找
    void clear();                              // 清空整棵树
    BTreeNode* getRoot();                      // 获取根节点

private:
    BTreeNode* root;
    void clearNode(BTreeNode* node);           // 递归释放节点
};

#endif // BTREE_H
