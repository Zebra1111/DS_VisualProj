#include "btree.h"


BTreeNode::BTreeNode(bool isLeaf) : isLeaf(isLeaf), numKeys(0) {
    for (int i = 0; i < 2 * DEGREE; i++) children[i] = nullptr;
}

BTreeNode::~BTreeNode() {
    for (int i = 0; i <= numKeys; i++) {
        delete children[i];
    }
}

void BTreeNode::insertNonFull(int key) {
    int i = numKeys - 1;
    if (isLeaf) {
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = key;
        numKeys++;
    } else {
        while (i >= 0 && keys[i] > key)
            i--;
        if (children[i + 1]->numKeys == 2 * DEGREE - 1) {
            splitChild(i + 1, children[i + 1]);
            if (key > keys[i + 1]) i++;
        }
        children[i + 1]->insertNonFull(key);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    BTreeNode* z = new BTreeNode(y->isLeaf);
    z->numKeys = DEGREE - 1;

    for (int j = 0; j < DEGREE - 1; j++)
        z->keys[j] = y->keys[j + DEGREE];

    if (!y->isLeaf) {
        for (int j = 0; j < DEGREE; j++)
            z->children[j] = y->children[j + DEGREE];
    }

    y->numKeys = DEGREE - 1;

    for (int j = numKeys; j >= i + 1; j--)
        children[j + 1] = children[j];

    children[i + 1] = z;

    for (int j = numKeys - 1; j >= i; j--)
        keys[j + 1] = keys[j];

    keys[i] = y->keys[DEGREE - 1];
    numKeys++;
}

BTreeNode* BTreeNode::search(int key) {
    int i = 0;
    while (i < numKeys && key > keys[i]) i++;
    if (i < numKeys && keys[i] == key) return this;
    if (isLeaf) return nullptr;
    return children[i]->search(key);
}

void BTreeNode::remove(int key) {
    int idx = 0;
    while (idx < numKeys && keys[idx] < key) ++idx;
    if (idx < numKeys && keys[idx] == key) {
        if (isLeaf) removeFromLeaf(idx);
        else removeFromNonLeaf(idx);
    } else {
        if (isLeaf) return;
        bool flag = (idx == numKeys);
        if (children[idx]->numKeys < DEGREE)
            fill(idx);
        if (flag && idx > numKeys)
            children[idx - 1]->remove(key);
        else
            children[idx]->remove(key);
    }
}

void BTreeNode::removeFromLeaf(int idx) {
    for (int i = idx + 1; i < numKeys; ++i)
        keys[i - 1] = keys[i];
    numKeys--;
}

void BTreeNode::removeFromNonLeaf(int idx) {
    int key = keys[idx];
    if (children[idx]->numKeys >= DEGREE) {
        int pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred);
    } else if (children[idx + 1]->numKeys >= DEGREE) {
        int succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ);
    } else {
        merge(idx);
        children[idx]->remove(key);
    }
}

int BTreeNode::getPred(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->isLeaf) cur = cur->children[cur->numKeys];
    return cur->keys[cur->numKeys - 1];
}

int BTreeNode::getSucc(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->isLeaf) cur = cur->children[0];
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->numKeys >= DEGREE)
        borrowFromPrev(idx);
    else if (idx != numKeys && children[idx + 1]->numKeys >= DEGREE)
        borrowFromNext(idx);
    else {
        if (idx != numKeys) merge(idx);
        else merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    for (int i = child->numKeys - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->isLeaf) {
        for (int i = child->numKeys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = keys[idx - 1];

    if (!child->isLeaf)
        child->children[0] = sibling->children[sibling->numKeys];

    keys[idx - 1] = sibling->keys[sibling->numKeys - 1];

    child->numKeys++;
    sibling->numKeys--;
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys[child->numKeys] = keys[idx];

    if (!child->isLeaf)
        child->children[child->numKeys + 1] = sibling->children[0];

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->numKeys; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->numKeys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->numKeys++;
    sibling->numKeys--;
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys[DEGREE - 1] = keys[idx];

    for (int i = 0; i < sibling->numKeys; ++i)
        child->keys[i + DEGREE] = sibling->keys[i];

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; ++i)
            child->children[i + DEGREE] = sibling->children[i];
    }

    for (int i = idx + 1; i < numKeys; ++i)
        keys[i - 1] = keys[i];

    for (int i = idx + 2; i <= numKeys; ++i)
        children[i - 1] = children[i];

    child->numKeys += sibling->numKeys + 1;
    numKeys--;

    delete sibling;
}

BTree::BTree() {
    root = nullptr;
}

BTree::~BTree() {
    clear();
}

void BTree::insert(int key) {
    if (!root) {
        root = new BTreeNode(true);
        root->keys[0] = key;
        root->numKeys = 1;
    } else {
        if (root->numKeys == 2 * DEGREE - 1) {
            BTreeNode* s = new BTreeNode(false);
            s->children[0] = root;
            s->splitChild(0, root);

            int i = (s->keys[0] < key) ? 1 : 0;
            s->children[i]->insertNonFull(key);

            root = s;
        } else {
            root->insertNonFull(key);
        }
    }
}

void BTree::remove(int key) {
    if (!root) return;

    root->remove(key);

    if (root->numKeys == 0) {
        BTreeNode* tmp = root;
        if (root->isLeaf)
            root = nullptr;
        else
            root = root->children[0];
        delete tmp;
    }
}

BTreeNode* BTree::search(int key) {
    return (root == nullptr) ? nullptr : root->search(key);
}

void BTree::clear() {
    clearNode(root);
    root = nullptr;
}

void BTree::clearNode(BTreeNode* node) {
    if (!node) return;
    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++)
            clearNode(node->children[i]);
    }
    delete node;
}

BTreeNode* BTree::getRoot() {
    return root;
}
