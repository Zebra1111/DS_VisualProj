#include "queue.h"

Queue::Queue() : frontIndex(0), rearIndex(0), size(0) {}

void Queue::enqueue(int value) {
    data[rearIndex] = value;
    rearIndex = (rearIndex + 1) % MAX_SIZE;
    size++;
}

void Queue::dequeue() {
    frontIndex = (frontIndex + 1) % MAX_SIZE;
    size--;
}

void Queue::clear() {
    frontIndex = rearIndex = size = 0;
}

QVector<int> Queue::toVector() const {
    QVector<int> result;
    for (int i = 0, idx = frontIndex; i < size; ++i, idx = (idx + 1) % MAX_SIZE)
        result.append(data[idx]);
    return result;
}
