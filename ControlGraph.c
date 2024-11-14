#include "ControlGraph.h"
#include "parser.h"

Subroutine CreateCFG(char* fileName, ParseTree * tree) {


   if (tree) {
     // get the funcDefs, attach each one to a subroutine and get for each subroutine the required details
       // construct the proper cfg for each subroutine
       tree = tree->children[0];
       Subroutine** subprograms=(Subroutine **)malloc(sizeof(Subroutine *)*tree->childrenCount);
       for (int i = 0; i < tree->childrenCount; i++) {
           subprograms[i] = (Subroutine*)malloc(sizeof(Subroutine));
           ParseTree* funcSigNode = tree->children[i]->children[0]->children[0];
           if (funcSigNode) {
              
               char* name = funcSigNode->token;
               Position* pos = (Position*)malloc(sizeof(Position));
               pos->line = funcSigNode->line;
               pos->width = funcSigNode->position;

               subprograms[i]->signatureDetails = (SignatureDetails*)malloc(sizeof(SignatureDetails));
               if (tree->children[i]->children[0]->childrenCount>1) {
                   subprograms[i]->signatureDetails->returnType = HandleType(tree->children[i]->children[0]->children[1]);
               }
                   //list arg defs
               if (funcSigNode->childrenCount != 0) {
                   ParseTree* argsTree = funcSigNode->children[0];
                   if (argsTree) {
                       subprograms[i]->signatureDetails->arguments = (ArgumentDef**)malloc(sizeof(ArgumentDef*) * argsTree->childrenCount);
                       for (int j = 0; j < argsTree->childrenCount; j++) {
                           char* temp = argsTree->children[j]->children[0]->token;
                           subprograms[i]->signatureDetails->arguments[j] = (ArgumentDef*)malloc(sizeof(ArgumentDef));
                           if (temp) {
                               subprograms[i]->signatureDetails->arguments[i]->name = name;
                               //setName(args[j]->name, name);
                           }

                           if (argsTree->childrenCount > 1) {

                               subprograms[i]->signatureDetails->arguments[j]->type = HandleType(argsTree->children[1]);
                           }

                         
                           // TODO: Create the CFG
                           // go about the creating a .dot file, I can check the makedot function and do something similar
                           // create a block for every statement (or child) of the funcDef, these blocks will depend on the type
                           // if the type is an if , then it will have two childeren, and if it is a while it will have a return
                           // if it is a break it ...
                           // in short, the types of blocks (or nodes in the cfg are 
                           //1. if
                           //2. while
                           //3. basic
                           //4. break
                           //5. do while
                           //and so the first step is MAKE A GRAPH -> func 1
                           //2: traverse it  and produce a .dot file -> func 2
                           // TODO: make this work for multiple files instead of 1
                           // TODO: after creating the CFG graph, then create a corresponding operations tree for each block
                           // this step just gives the l value and r value if there is an expr
                           //TODO: capture the errors
                           //the error types are ->
                           //1. break into nothing
                           //2. assign into a literal, (literal is an lvalue in code)
                           //3. use a function that is not initialized (not in the cfg already)
                            // return the errors as well 

                       }
                       /*  = args;*/
                   }
               }
               subprograms[i]->signatureDetails->position = pos;
               subprograms[i]->name = name;
               
               
           }



       }
    }

}
Type* create_simple_type(SimpleType simple_type, const char* custom_id) {
    Type* type = (Type*)malloc(sizeof(Type));
    type->kind = TYPE_SIMPLE;
    type->data.simpleType.type = simple_type;

    if (custom_id) {
        strncpy(type->data.simpleType.custom_id, custom_id, sizeof(type->data.simpleType.custom_id) - 1);
        type->data.simpleType.custom_id[sizeof(type->data.simpleType.custom_id) - 1] = '\0'; // Ensure null-termination
    }
    else {
        type->data.simpleType.custom_id[0] = '\0'; // Default empty string
    }

    return type;
}
Type* create_array_type(Type* element_type, size_t length) {
    Type* type = (Type*)malloc(sizeof(Type));
    type->kind = TYPE_ARRAY;
    type->data.arrayType.elementType = element_type;
    type->data.arrayType.length = length;
    return type;
}
void free_type(Type* type) {
    if (type) {
        if (type->kind == TYPE_ARRAY) {
            free_type(type->data.arrayType.elementType); // Free the element type if it's a type that was allocated
        }
        free(type); // Free the type itself
    }
}
Type* HandleType(ParseTree* typeNode) {

    Type* returnType = (Type*)malloc(sizeof (Type));
    if (typeNode->childrenCount != 1) {
        int size = typeNode->children[1]->childrenCount;
        return create_array_type(HandleType(typeNode->children[0]->children[0]), size);
    }
    else {
        char* typeName = typeNode->token;
      
        if (typeName == "int") {
            return create_simple_type(TYPE_INT,"");

        }
        if (typeName == "bool") {
            return create_simple_type(TYPE_BOOL, "");

        }
        if (strcmp(typeName, "byte") == 0) {
            return create_simple_type(TYPE_BYTE, "");
        }
        if (strcmp(typeName, "uint") == 0) {
            return create_simple_type(TYPE_UINT, "");
        }
        if (strcmp(typeName, "long") == 0) {
            return create_simple_type(TYPE_LONG, "");
        }
        if (strcmp(typeName, "ulong") == 0) {
            return create_simple_type(TYPE_ULONG, "");
        }
        if (strcmp(typeName, "char") == 0) {
            return create_simple_type(TYPE_CHAR, "");
        }
        if (strcmp(typeName, "string") == 0) {
            return create_simple_type(TYPE_STRING, "");
        }
        // Custom type handling can be extended as needed
       
            return create_simple_type(TYPE_CUSTOM, typeName);
        
    }
}
