#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include <QList>

struct SinglyListNode {
    int value;
    SinglyListNode *next;

    SinglyListNode(int val) : value(val), next(nullptr) {}
};

class SinglyLinkedList {
public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    void insertAtHead(int value);   // 头插
    void insertAtTail(int value);   // 尾插
    void deleteValue(int value);    // 删除第一个匹配值
    bool find(int value) const;
    void clear();                   // 清空链表
    SinglyListNode* getHead() const;
    QList<int> toList() const;      // 转为 QList<int> 用于可视化

private:
    SinglyListNode *head;
};

#endif // SINGLYLINKEDLIST_H
