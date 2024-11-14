#pragma once
#include "parser.h"
////
// handeling the type problem
/////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Function to create a simple type
Type* create_simple_type(SimpleType simple_type, const char* custom_id); 

// Function to create an array type
Type* create_array_type(Type* element_type, size_t length); 

// Function to free the Type
void free_type(Type* type);
Type* HandleType(ParseTree* typeNode);

//// Example usage
//int main() {
//    // Create a simple custom type
//    Type* customType = create_simple_type(TYPE_CUSTOM, "MyCustomID");
//
//    // Create an array of the custom type with length 5
//    Type* arrayType = create_array_type(customType, 5);
//
//    // Print out details
//    if (arrayType->kind == TYPE_ARRAY) {
//        printf("Array Length: %zu
//            ", arrayType->data.arrayType.length);
//            printf("Element Type: Custom ID = %s
//                ", arrayType->data.arrayType.elementType->data.simpleType.custom_id);
//    }
//
//    // Clean up
//    free_type(arrayType); // This will also free the customType
//
//    return 0;
//}

/// <summary>
/// Represents a definition of an argument, including its name and type.
/// </summary>
typedef struct ArgumentDef {
    char* name;  ///< The name of the argument.
    Type* type;  ///< The type of the argument (e.g., int, float, etc.).
} ArgumentDef;
void setName(ArgumentDef* arg, char* name);

/// <summary>
/// Represents the position in the source code, specified by line and column width.
/// </summary>
typedef struct Position {
    int line;   ///< The line number in the source code.
    int width;  ///< The column number (width) in the line.
} Position;


/// <summary>
/// Contains details about a function or method signature, including its name,
/// arguments, return type, and position in the source code.
/// </summary>
typedef struct SignatureDetails {
    struct ArgumentDef** arguments;  ///< An array of argument definitions for the function/method.
    struct Type* returnType;      ///< The return type of the function/method.
    struct Position *position; ///< The position of the signature in the source code.
} SignatureDetails;


/// <summary>
/// Enum representing the different types of control flow graph blocks.
/// </summary>
enum BlockType {
    BaseBlock,          ///< A basic block, no special control flow.
    WhileBlock,        ///< A block representing a while loop.
    IfBlock,           ///< A block representing an if statement.
    DoWhileBlock,      ///< A block representing a do-while loop.
    BreakBlock,        ///< A block representing a break statement.
    ExpressionBlock    ///< A block representing an expression.
};


/// <summary>
/// Represents a block in the control flow graph, including its type and links
/// to other blocks (incoming and outgoing nodes).
/// </summary>
typedef struct controlFlowGraphBlock {
    enum BlockType blocktype;                 ///< The type of the control flow graph block.
    struct controlFlowGraphBlock* outNodes;  ///< Pointer to the outgoing nodes (following blocks).
    struct controlFlowGraphBlock* inNodes;   ///< Pointer to the incoming nodes (preceding blocks).
} controlFlowGraphBlock;


/// <summary>
/// Represents a subroutine (function/method), including its name, control flow
/// graph, and signature details.
/// </summary>
typedef struct Subroutine {
    char* name;                            ///< The name of the subroutine.
    struct controlFlowGraphBlock* cfg;    ///< Pointer to the control flow graph associated with the subroutine.
    struct SignatureDetails* signatureDetails; ///< Pointer to the signature details of the subroutine.
} Subroutine;

/// <summary>
/// Creates a control flow graph from a file and a parse tree.
/// </summary>
/// <param name="fileName">The name of the file to process.</param>
/// <param name="tree">The parse tree to use for creating the control flow graph.</param>
/// <returns>A Subroutine object representing the created control flow graph.</returns>
Subroutine CreateCFG(char* fileName, ParseTree* tree);
