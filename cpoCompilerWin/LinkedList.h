#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef struct Node {
    void* data;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
    size_t size;
} LinkedList;

// Function prototypes
LinkedList* createLinkedList();
void freeLinkedList(LinkedList* list);
int insertNode(LinkedList* list, void* data);
void* getNodeData(LinkedList* list, size_t index);
size_t getListSize(LinkedList* list);
void removeNode(LinkedList* list, size_t index);
void clearList(LinkedList* list);
#endif // LINKED_LIST_H;
