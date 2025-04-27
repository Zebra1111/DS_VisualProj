#include "singlylist.h"

SinglyLinkedList::SinglyLinkedList() : head(nullptr) {}

SinglyLinkedList::~SinglyLinkedList() {
    clear();
}

void SinglyLinkedList::insertAtHead(int value) {
    SinglyListNode *newNode = new SinglyListNode(value);
    newNode->next = head;
    head = newNode;
}

void SinglyLinkedList::insertAtTail(int value) {
    SinglyListNode *newNode = new SinglyListNode(value);
    if (!head) {
        head = newNode;
        return;
    }
    SinglyListNode *temp = head;
    while (temp->next)
        temp = temp->next;
    temp->next = newNode;
}

void SinglyLinkedList::deleteValue(int value) {
    SinglyListNode *curr = head, *prev = nullptr;

    while (curr && curr->value != value) {
        prev = curr;
        curr = curr->next;
    }

    if (!curr) return;

    if (!prev) {
        head = curr->next;
    } else {
        prev->next = curr->next;
    }

    delete curr;
}

bool SinglyLinkedList::find(int value) const {
    SinglyListNode *curr = head;
    while (curr) {
        if (curr->value == value)
            return true;
        curr = curr->next;
    }
    return false;
}


void SinglyLinkedList::clear() {
    while (head) {
        SinglyListNode *temp = head;
        head = head->next;
        delete temp;
    }
}

SinglyListNode* SinglyLinkedList::getHead() const {
    return head;
}

QList<int> SinglyLinkedList::toList() const {
    QList<int> result;
    SinglyListNode *curr = head;
    while (curr) {
        result.append(curr->value);
        curr = curr->next;
    }
    return result;
}
