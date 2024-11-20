#pragma once

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------------
// Enumerations
// -------------------------

// Define an enumeration for simple types
typedef enum {
    TYPE_BOOL,
    TYPE_BYTE,
    TYPE_INT,
    TYPE_UINT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_CUSTOM // For custom types (e.g., ID)
} SimpleType;

// Enum representing the different types of control flow graph blocks.
typedef enum BlockType {
    BaseBlock,    ///< A basic block, no special control flow.
    WhileBlock,   ///< A block representing a while loop.
    IfBlock,      ///< A block representing an if statement.
    BreakBlock,   ///< A block representing a break statement.
    ElseBlock,
    ThenBlock,
    WhileBodyBlock
} BlockType;

typedef enum {
    NODE_TYPE_OPERATOR,
    NODE_TYPE_OPERAND
} OTNodeType;

typedef enum {
    TYPE_OTNODE,
    TYPE_VARDECLARATION
} ContentType;

// -------------------------
// Structures
// -------------------------

// Define a struct to represent simple types
typedef struct {
    SimpleType type;
    char custom_id[32]; // Hold custom ID as a string
} SimpleTypeStruct;

// Define a struct to represent array types
typedef struct Array {
    struct Type* elementType; // Pointer to the type of elements in the array
    size_t length;            // Number of elements
} ArrayType;

// Define a union to represent either a simple type or an array type
typedef union {
    SimpleTypeStruct simpleType;
    ArrayType arrayType;
} TypeData;

// Define a struct to encapsulate our Type
typedef struct Type {
    enum {
        TYPE_SIMPLE,
        TYPE_ARRAY
    } kind;               // To keep track of whether it's a simple type or array
    TypeData data;       // Union to hold the type data
} Type;

// Define the argument definition structure
typedef struct ArgumentDef {
    char* name;  ///< The name of the argument.
    struct Type* type;  ///< The type of the argument (e.g., int, float, etc.).
} ArgumentDef;

// Define a position structure to represent the source code location
typedef struct Position {
    int line;   ///< The line number in the source code.
    int width;  ///< The column number (width) in the line.
} Position;

// Define a structure for function signature details
typedef struct SignatureDetails {
    struct ArgumentDef** arguments;  ///< An array of argument definitions for the function/method.
    struct Type* returnType;      ///< The return type of the function/method.
    struct Position* position; ///< The position of the signature in the source code.
} SignatureDetails;

// Structure to represent a variable declaration
typedef struct varDeclaration {
    char** Ids;
    Type* type;
} varDeclaration;

// Function to create a variable declaration
varDeclaration* CreateVarDeclaration(char** Ids, Type* type);

// Structure to represent an operation tree node
typedef struct OTNode {
    enum OTNodeType type;     // Type of the node (operator or operand)
    union {
        char* operator; // char to represent operators (+, -, *, /)
        char* operand;   // int to represent numeric values
    } value;          // Union to hold either an operator or an operand
    struct OTNode* left;  // Pointer to the left child
    struct OTNode* right; // Pointer to the right child
} OTNode;

// Structure to hold control flow graph block content
typedef struct cfgBlockContent {
    ContentType type;  // Track the type of content
    union {
        struct OTNode* ot;
        struct varDeclaration* varDec;
    };
}cfgBlockContent;

// Structure to represent instructions
typedef struct Instructions {
    struct cfgBlockContent** content;
    int size;
}Instructions;

typedef struct controlFlowGraphBlock {
    struct ParseTree* ast;                         //< the corresponding AST
    enum BlockType blocktype;                    ///< The type of the control flow graph block.
    struct controlFlowGraphBlock** nodes;   ///< Pointer to the outgoing nodes (following blocks).
    //struct controlFlowGraphBlock** inNodes;   ///< Pointer to the outgoing nodes (following blocks).
    //int inNodeCount;                        ///< Number of incoming nodes (preceding blocks).
    int outNodeCount;                       ///< Number of outgoing nodes (following blocks).
    Instructions* instructions;             // CFG Block Content
} controlFlowGraphBlock, * pControlFlowGraphBlock;


typedef struct Subroutine {
    char* name;                            ///< The name of the subroutine.
    struct controlFlowGraphBlock* cfg;    ///< Pointer to the control flow graph associated with the subroutine.
    struct SignatureDetails* signatureDetails; ///< Pointer to the signature details of the subroutine.
}Subroutine;


// a stack cuz why not 

typedef struct Stack {
    struct controlFlowGraphBlock** items; // Array of pointers to controlFlowGraphBlocks
    int top;                        // Index of the top element
    int capacity;                   // Capacity of the stack
} Stack;

// Function to create a stack
Stack* createStack();
// Function to check if the stack is empty
int isEmpty(Stack* stack);
// Function to double the capacity of the stack
void resizeStack(Stack* stack);
// Function to push an item onto the stack
void push(Stack* stack, controlFlowGraphBlock* block);
// Function to pop an item from the stack
controlFlowGraphBlock* pop(Stack* stack);
// Function to peek at the top item of the stack without removing it
controlFlowGraphBlock* peek(Stack* stack);
// Function to delete the stack and free its memory
void deleteStack(Stack* stack);
// Argument definition functions
void setName(ArgumentDef* arg, char* name);

// Control flow graph functions
void insertCFGBlock(controlFlowGraphBlock* nodes, controlFlowGraphBlock* node);
controlFlowGraphBlock** createCFGBlock(controlFlowGraphBlock* block);
void InsertInstruction(Instructions* instructions, cfgBlockContent NewContent);
Instructions* CreateInstructions( );
Subroutine ** DefineSubprogram(char* fileName, controlFlowGraphBlock** cfgs,ParseTree* tree);
void ConstructCFG(controlFlowGraphBlock* cfg, ParseTree* tree, BlockType blockType);
controlFlowGraphBlock** CFGInterfacer(char* fileName, ParseTree* tree);
controlFlowGraphBlock* writeDotGraph(Stack* openNodes, FILE* file);
void CFGToDotFile(controlFlowGraphBlock* cfgs, char* fileName);

// Operation tree functions
OTNode* createOperatorNode(char* operator);
OTNode* createOperandNode(char* operand);
void freeTree(OTNode* root);
char* printTree(char* treeText,OTNode* node);
Type* HandleType(ParseTree* typeNode);