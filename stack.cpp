#include "stack.h"

Stack::Stack() : topIndex(-1) {}

void Stack::push(int value) {
    data[++topIndex] = value;
}

void Stack::pop() {
    topIndex--;
}


void Stack::clear() {
    topIndex = -1;
}

QVector<int> Stack::toVector() const {
    QVector<int> result;
    for (int i = topIndex; i >= 0; --i) {
        result.append(data[i]);
    }
    return result;
}
