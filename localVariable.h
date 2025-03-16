#pragma once
#pragma once
#include "ControlGraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VarNode structure for the linked list
typedef struct VarNode {
    //struct varDeclaration* data;
    char* id;
    struct Type* type;
    int offset;
    int cntVars;
    struct VarNode* next;
    char* fileName;
} VarNode;
typedef struct FunctionVariables {

    VarNode* localVariables;
    VarNode* parameters; // signature variables
    char* funcName;
    int cntVars;
    int cntArgs;

}FunctionVariables;

// Function to create a new node
VarNode* createNode(char* id, Type* type ,  char  *fileName);

// Function to add a node to the end of the linked list
void addNode(VarNode** head, char * id, Type * type,int offset, char * fileName);

// Function to free the linked list
void freeList(VarNode* head);



/// <summary>
/// this is for the functions linked list
/// </summary>
/// <param name="subroutines"></param>
/// <param name="cnt"></param>
/// <param name="head"></param>
/// <param name="fileName"></param>
/// <returns></returns>
/// 
/// 
// Function to create a new node
FunctionVariables* createFuncVar( FunctionVariables** head,VarNode* localVars, VarNode * parameters, char* fileName);

// Function to add a node to the end of the linked list
void addFuncVar(FunctionVariables** head, FunctionVariables* funcVar, char* fileName);

// Function to free the linked list
void freeFuncVars(FunctionVariables* head);



FunctionVariables** getLocalVars(Subroutine** subroutines, int cnt, char* fileName);
VarNode* newVar(controlFlowGraphBlock* node, VarNode* var, char* fileName);
VarNode* traverseCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, VarNode* var , char* fileName);
VarNode* traverseCfgWhileStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, VarNode* var , char* fileName);
VarNode* traverseCfgBaseStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, VarNode* var , char* fileName);
VarNode* traverseCfg(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start, VarNode* var , char* fileName);

