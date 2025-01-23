#pragma once
#pragma once
#include "ControlGraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node structure for the linked list
typedef struct Node {
    struct varDeclaration* data;
    struct Node* next;
    char* fileName;
} Node;
// Function to create a new node
Node* createNode(varDeclaration * data, char  *fileName);

// Function to add a node to the end of the linked list
void addNode(Node** head, varDeclaration * data, char * fileName);

// Function to free the linked list
void freeList(Node* head);
Node* getLocalVars(Subroutine** subroutines, int cnt, Node* head, char * fileName);
Node* newVar(controlFlowGraphBlock* node, Node* var,  char* fileName);
Node* traverseCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, Node* var, char* fileName);
Node* traverseCfgWhileStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, Node* var, char* fileName);
Node* traverseCfgBaseStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, Node* var, char* fileName);
Node* traverseCfg(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start, Node* var, char* fileName);

