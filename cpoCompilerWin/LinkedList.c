#include <stdio.h>
#include <stdlib.h>
#include "LinkedList.h"
// Function to create a new linked list
LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->size = 0;
    return list;
}

// Function to free all nodes in the linked list
static void freeNodes(Node* node) {
    if (node) {
        freeNodes(node->next);
        free(node);
    }
}

// Function to free the linked list
void freeLinkedList(LinkedList* list) {
    if (list) {
        freeNodes(list->head);
        free(list);
    }
}

// Function to insert a node into the linked list
int insertNode(LinkedList* list, void* data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        return 0; // memory allocation failed
    }
    newNode->data = data;
 
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
    return 1;
}

// Function to get the data of a node at a specific index
void* getNodeData(LinkedList* list, size_t index) {
    if (index >= list->size) {
        return NULL; // index out of bounds
    }
    Node* current = list->head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

// Function to return the size of the linked list
size_t getListSize(LinkedList* list) {
    return list->size;
}

// Function to remove a node at a specific index
void removeNode(LinkedList* list, size_t index) {
    if (index >= list->size) {
        return; // index out of bounds
    }
    Node* current = list->head;
    if (index == 0) {
        list->head = current->next;
        free(current);
    }
    else {
        Node* prev = NULL;
        for (size_t i = 0; i < index; i++) {
            prev = current;
            current = current->next;
        }
        prev->next = current->next;
        free(current);
    }
    list->size--;
}

// Function to clear the linked list
void clearList(LinkedList* list) {
    freeNodes(list->head);
    list->head = NULL;
    list->size = 0;
}