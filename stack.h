#ifndef STACK_H
#define STACK_H

#include <QVector>
#include <QDebug>

class Stack {
public:
    Stack();
    void push(int value);
    void pop();
    int top() const;
    bool isEmpty() const;
    bool isFull() const;
    void clear();
    QVector<int> toVector() const;

private:
    static const int MAX_SIZE = 1000;
    int data[MAX_SIZE];
    int topIndex;
};

#endif // STACK_H
