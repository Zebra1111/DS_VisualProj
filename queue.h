#ifndef QUEUE_H
#define QUEUE_H

#include <QVector>

class Queue {
public:
    Queue();
    void enqueue(int value);
    void dequeue();
    void clear();
    QVector<int> toVector() const;

private:
    static const int MAX_SIZE = 1000;
    int data[MAX_SIZE];
    int frontIndex;
    int rearIndex;
    int size;
};

#endif // QUEUE_H
