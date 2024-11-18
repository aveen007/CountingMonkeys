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
    ThenBlock
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
    Type* type;  ///< The type of the argument (e.g., int, float, etc.).
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
    OTNodeType type;     // Type of the node (operator or operand)
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
        OTNode* ot;
        varDeclaration* varDec;
    };
}cfgBlockContent;

// Structure to represent instructions
typedef struct Instructions {
    cfgBlockContent** content;
    int size;
}Instructions;

typedef struct controlFlowGraphBlock {
    ParseTree* ast;                         //< the corresponding AST
    BlockType blocktype;                    ///< The type of the control flow graph block.
    struct controlFlowGraphBlock** nodes;   ///< Pointer to the outgoing nodes (following blocks).
    //int inNodeCount;                        ///< Number of incoming nodes (preceding blocks).
    int outNodeCount;                       ///< Number of outgoing nodes (following blocks).
    Instructions* instructions;             // CFG Block Content
} controlFlowGraphBlock, * pControlFlowGraphBlock;


typedef struct Subroutine {
    char* name;                            ///< The name of the subroutine.
    struct controlFlowGraphBlock* cfg;    ///< Pointer to the control flow graph associated with the subroutine.
    struct SignatureDetails* signatureDetails; ///< Pointer to the signature details of the subroutine.
}Subroutine;

// -------------------------
// Function Prototypes
// -------------------------

// Type handling functions
Type* create_simple_type(SimpleType simple_type, const char* custom_id);
Type* create_array_type(Type* element_type, size_t length);
void free_type(Type* type);
Type* HandleType(ParseTree* typeNode);

// Argument definition functions
void setName(ArgumentDef* arg, char* name);

// Control flow graph functions
void insertCFGBlock(controlFlowGraphBlock* nodes, controlFlowGraphBlock* node);
controlFlowGraphBlock** createCFGBlock(controlFlowGraphBlock* block);
void InsertInstruction(Instructions* instructions, cfgBlockContent NewContent);
Instructions* CreateInstructions( );
Subroutine DefineSubprogram(char* fileName, ParseTree* tree);
void ConstructCFG(controlFlowGraphBlock* cfg, ParseTree* tree, BlockType blockType);
void CFGInterfacer(char* fileName, ParseTree* tree);
void writeDotGraph(controlFlowGraphBlock* cfg, FILE* file);
void CFGToDotFile(controlFlowGraphBlock* cfgs,  char* fileName);

// Operation tree functions
OTNode* createOperatorNode(char* operator);
OTNode* createOperandNode(char* operand);
void freeTree(OTNode* root);
void printTree(OTNode* node);
