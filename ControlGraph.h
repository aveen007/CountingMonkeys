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
    TYPE_CUSTOM, // For custom types (e.g., ID)
    
} SimpleType;
static const char* SimpleType_STRING[] = {

    "TYPE_BOOL",
     "TYPE_BYTE",
     "TYPE_INT",
     "TYPE_UINT",
     "TYPE_LONG",
     "TYPE_ULONG",
     "TYPE_CHAR",
     "TYPE_STRING",
     "TYPE_CUSTOM",
};
// Enum representing the different types of control flow graph blocks.

typedef enum BlockType {
    BaseBlock,
    WhileBlock,
    IfBlock,
    BreakBlock,
    ElseBlock,
    ThenBlock,
    IfExitBlock,
    WhileBodyBlock,
    WhileExitBlock
} BlockType;
static const char* BlockType_STRING[] = {
    "BaseBlock",
    "WhileBlock",
    "IfBlock",
    "BreakBlock",
    "ElseBlock",
    "ThenBlock",
    "IfExitBlock",
    "WhileBodyBlock",
    "WhileExitBlock"
};
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

typedef struct GenericType {
    char* name;
    struct Type** parameters;
    int parametersCount;
} GenericType;

// Define a union to represent either a simple type or an array type
typedef union {
    SimpleTypeStruct simpleType;
    ArrayType arrayType;
    GenericType genericType;
} TypeData;

// Define a struct to encapsulate our Type
typedef struct Type {
    enum {
        TYPE_SIMPLE,
        TYPE_GENERIC,
        TYPE_ARRAY,
        TYPE_NONE
    } kind;               // To keep track of whether it's a simple type or array
    TypeData data;       // Union to hold the type data
    struct classDef* def; // not NULL when references custom type
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
    int cntArgs;
} SignatureDetails;

// Structure to represent a variable declaration
typedef struct varDeclaration {
    char** Ids;
    struct Type* type;
    int cntId;
} varDeclaration;

// Function to create a variable declaration
varDeclaration* CreateVarDeclaration(char** Ids,int cnt, Type* type);

// Structure to represent an operation tree node
typedef struct OTNode {
    OTNodeType type;     // Type of the node (operator or operand)
    union {
        char* operator; // char to represent operators (+, -, *, /)
        char* operand;   // int to represent numeric values
    } value;          // Union to hold either an operator or an operand
    struct OTNode** operands;  // Pointer to the left child
    int cntOperands;
    //struct OTNode* right; // Pointer to the right child
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
    int outNodeCount;                       ///< Number of outgoing nodes (following blocks).
    Instructions* instructions;             // CFG Block Content
    struct callGraph* called;
    int drawn;
} controlFlowGraphBlock, * pControlFlowGraphBlock;

typedef struct Subroutine {
    char* name;                            ///< The name of the subroutine.
    struct controlFlowGraphBlock* cfg;    ///< Pointer to the control flow graph associated with the subroutine.
    struct SignatureDetails* signatureDetails; ///< Pointer to the signature details of the subroutine.
}Subroutine;

typedef struct ErrorInfoCFG {
    struct ErrorInfoCFG* next;
    char* message;
    struct ParseTree* ast;
    int line;
    int position;
} ErrorInfoCFG;

typedef struct CfgsInfo {
    controlFlowGraphBlock** cfgs;
    ErrorInfoCFG* errors;
    struct classDefInfo* classes;

} CfgsInfo;

CfgsInfo* createCfgsInfo(ParseTree* ast);
controlFlowGraphBlock** createCfgs(ParseTree* ast);
Subroutine** createSubroutines(ParseTree* ast);
void freeErrorInfoCFG(ErrorInfoCFG* head);
void addErrorCFG(ErrorInfoCFG** errors, ErrorInfoCFG* newError);
ErrorInfoCFG* createErrorInfoNodeCFG(const char* message, int line, int position);
// a stack cuz why not 

typedef struct Stack {
    struct ParseTree** items; // Array of pointers to controlFlowGraphBlocks
    int top;                        // Index of the top element
    int capacity;                   // Capacity of the stack
} Stack;

typedef struct callGraph {
    //struct controlFlowGraphBlock* node;
    struct controlFlowGraphBlock** calledProcedures;
    int cntNames;
    char** calledTokens;

    int cnt;
}callGraph;
typedef struct cfgFile {
    char* name;
    struct ParseTree* ast;
    struct CfgsInfo* cfgs;
    int cntCfgs;

}cfgFile;

#pragma region classes
typedef enum {
    MODIFIER_PUBLIC,
    MODIFIER_PRIVATE,
    MODIFIER_PROTECTED
} AccessModifier;

typedef struct FunctionInfo {
    enum AccessModifier modifier;
    int offset;
    struct Subroutine* subroutine;  // For internal functions
} FunctionInfo;

typedef struct ExternalFunctionInfo {
    enum AccessModifier modifier;
    int offset;
    struct ExternFuncDef* externalFunction;
} ExternalFunctionInfo;

typedef struct ArgumentInfo {
    enum AccessModifier modifier;
    int offset;
    struct ArgumentDef* argument;
} ArgumentInfo;




// class stuff
typedef struct classDef {

    char* name;
    char** parameterNames;
    //classDef** parameters;

    int parametersCount;
   // bool isGenericParameter;

    Type* baseType;
    
    // Internal functions
    FunctionInfo** functions;
    int functionCount;

    // External functions
    ExternalFunctionInfo** externalFunctions;
    int externalFunctionCount;

    // Arguments
    ArgumentInfo** arguments;
    int argumentCount;

}classDef;
typedef struct classDefInfo {

    struct  classDef** classes;
    int classCount;
}classDefInfo;
// externFuncDef: 'declare' 'function' funcSignature 'lib' dllName('alias'
//    dllEntryName) ? {
//};

typedef struct ExternFuncDef {
    char* name;                            ///< The name of the subroutine.
    //struct controlFlowGraphBlock* cfg;    ///< Pointer to the control flow graph associated with the subroutine.
    struct SignatureDetails* signatureDetails; ///< Poin
    char* dllName;
    char* dllEntryName;
}ExternFuncDef;

typedef struct subroutineInfo {
    Subroutine** subroutines;
    int count;
}subroutineInfo;
ExternFuncDef* createExternFuncDef(ParseTree* ast);







// Creates a new class definition
classDef* createClassDef(const char* name);
classDefInfo* createClassDefInfo();
classDefInfo * addClassDefInfo(classDefInfo* classes, classDef* classdef);
// Creates a new internal function info
FunctionInfo* createFunctionInfo(AccessModifier modifier, int offset, Subroutine* subroutine);

// Creates a new external function info
ExternalFunctionInfo* createExternalFunctionInfo(AccessModifier modifier, int offset, ExternFuncDef* externalFunction);

// Creates a new argument info
ArgumentInfo* createArgumentInfo(AccessModifier modifier, int offset, ArgumentDef* argument);



// Add internal function to class
void addFunctionToClass(classDef* cls, FunctionInfo* funcInfo);

// Add external function to class
void addExternalFunctionToClass(classDef* cls, ExternalFunctionInfo* extFuncInfo);

// Add argument to class
void addArgumentToClass(classDef* cls, ArgumentInfo* argInfo);
// Frees memory allocated for a classDef and all its contents
void freeClassDef(classDef* cls);

#pragma endregion


cfgFile** HandleCallGraphs(cfgFile** allFiles, int fileCnt);

void insertCG(controlFlowGraphBlock* caller, controlFlowGraphBlock* called);
void insertCGToken(char* called);
void printCallGraph(cfgFile** allFiles, int fileCnt);
controlFlowGraphBlock* FindCFG(controlFlowGraphBlock** cfgss, int cfgCnt, char* funcName);

callGraph* CreateCGNode(controlFlowGraphBlock* cfg);
// Function to create a stack
Stack* createStack();
// Function to check if the stack is empty
int isEmpty(Stack* stack);
// Function to double the capacity of the stack
void resizeStack(Stack* stack);
// Function to push an item onto the stack
void push(Stack* stack, ParseTree* block);
// Function to pop an item from the stack
ParseTree* pop(Stack* stack);
// Function to peek at the top item of the stack without removing it
ParseTree* peek(Stack* stack);
// Function to delete the stack and free its memory
void deleteStack(Stack* stack);
// Argument definition functions
void setName(ArgumentDef* arg, char* name);

// Control flow graph functions
void insertCFGBlock(controlFlowGraphBlock* nodes, controlFlowGraphBlock* node);
controlFlowGraphBlock* createCFGBlock(ParseTree* ast);
void InsertInstruction(Instructions* instructions, ParseTree* ast);
Instructions* CreateInstructions();
cfgBlockContent* createInstructionsVarStatement(ParseTree* ast);
cfgBlockContent* createInstructionsExpression(ParseTree* ast);
subroutineInfo* DefineSubprogram(char* fileName, controlFlowGraphBlock** cfgs, ParseTree* tree);




controlFlowGraphBlock* ConstructCFGWhileStatement(Stack* openNodes, controlFlowGraphBlock* start);
controlFlowGraphBlock* ConstructCFGIfStatement(Stack* openNodes, controlFlowGraphBlock* start);
controlFlowGraphBlock* ConstructCFGBaseStatement(Stack* openNodes, controlFlowGraphBlock* start);
controlFlowGraphBlock* ConstructCFG(Stack* openNodes, controlFlowGraphBlock* start);


CfgsInfo* CFGInterfacer(char* fileName, ParseTree* tree);
char* writeDotGraphOperationsTree(controlFlowGraphBlock* cfg, FILE* file);
void writeDotGraphIfStatement(controlFlowGraphBlock* node, FILE* file, controlFlowGraphBlock* start);
void writeDotGraphWhileStatement(controlFlowGraphBlock* node, FILE* file, controlFlowGraphBlock* start);
void writeDotGraphBaseStatement(controlFlowGraphBlock* node, FILE* file, controlFlowGraphBlock* start);
void printBlockToFile(char* blockType, FILE* file, controlFlowGraphBlock* node);
controlFlowGraphBlock** processCfg(ParseTree* tree, CfgsInfo* cfgsInfo, char* fileName);
Subroutine* createSubroutine();
SignatureDetails* createSignatureDetails(ParseTree* ast);
void writeDotGraph(controlFlowGraphBlock* cfg, FILE* file, controlFlowGraphBlock* start);
void CFGToDotFile(controlFlowGraphBlock* cfgs, char* fileName);

// Operation tree functions
OTNode* createOperatorNode(char* operator);
OTNode* createOperandNode(char* operand);
void freeTree(OTNode* root);
char* printTree(char* treeText, OTNode* node);
Type* HandleType(ParseTree* typeNode);
size_t estimatedSize(OTNode* node);
char* mystrcat(const char* str1, const char* str2);
int stringLen(char* str);
void CreateFilePrint(char* fileName, CfgsInfo* info, controlFlowGraphBlock* cfg);
void intToStr(int num, char* str);
char* remove_last_three_chars(const char* fileName);
Subroutine** addSubroutine(Subroutine** list, Subroutine* newSub);
Type* create_simple_type(SimpleType simple_type, const char* custom_id);
int addParameterName(classDef* cls, const char* paramName);
Type* create_generic_type(char* class_name, int param_count, ParseTree* base);
char* get_type_name(Type* type);