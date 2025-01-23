#include "asm.h"
#include "ControlGraph.h"
#include "parser.h"
#include "localVariable.h"



#define asm_code_header "[section ram, code]\n"
#define asm_data_header "data:\n"
#define asm_footer "halt:\n\thlt\n"

#define mnemonic_1(mnemonic, arg1) fprintf(asmCodeOut, "\t%s %s\n", mnemonic, arg1);
#define mnemonic_1_i(mnemonic, arg1) fprintf(asmCodeOut, "\t%s %d\n", mnemonic, arg1);
#define mnemonic_2(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %s, %s\n", mnemonic, arg1, arg2);
#define mnemonic_2_i(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %d, %s\n", mnemonic, arg1, arg2);
#define mnemonic_3(mnemonic, arg1, arg2, arg3) fprintf(asmCodeOut, "\t%s %s, %s, %s\n", mnemonic, arg1, arg2, arg3);
#define mnemonic_0(mnemonic) fprintf(asmCodeOut, "\t%s\n", mnemonic);

#define push(value) mnemonic_1("push", value)
#define push_i(value) mnemonic_1_i("push", value)
#define pop() mnemonic_0("pop")
#define iadd() mnemonic_0("iadd")
#define ladd(op1, op2, to) mnemonic_0("ladd")

#define jump(target) mnemonic_1("jump", target)
#define jz(target, cond) mnemonic_2("jz", target, cond)
#define store(value) mnemonic_1("store", value)
#define astore(value) mnemonic_1("astore", value)





FILE* asmCodeOut;
FILE* asmDataOut;
int labelCounter = 0;
char* labelName() {
    char* c = malloc(sizeof(char) * 32);
    sprintf(c, "label_%d", labelCounter++);
    return c;
}

char* labelNameIdent(char* ident) {
    char* c = malloc(sizeof(char) * 32);
    sprintf(c, "%s_%d", ident, labelCounter++);
    return c;
}

#define put_label(name) fprintf(asmCodeOut, "%s:\n", name);
#define put_comment(comment) fprintf(asmCodeOut, "\t\t;%s\n", comment);
#define put_comment_var(comment) fprintf(asmCodeOut, "\t\t;%s\n", comment);
#define put_label_var(name, type, value) \
    fprintf(asmDataOut, "%s: dd 0x0 \n", name); \
    fprintf(asmDataOut, "    .type: dw 0x%x ; Offset for `type`\n", type); \
    fprintf(asmDataOut, "    .value: dw 0x%d ; Offset for `value`\n", value);
int generateAsm(Node * localVars) {
    fprintf(asmCodeOut, asm_code_header);
    //fprintf(asmDataOut, asm_data_header);
    translate_var_declarations(localVars);
    fprintf(asmDataOut, asm_footer);
    jump("halt");

}
void translate_type(char* id,Type* type) {
    //here I push some encoding of the type 
    switch (type->kind)
    {
    case  TYPE_SIMPLE:
        switch (type->data.simpleType.type) {

        case    TYPE_CUSTOM:
            push_i(10);
            break;// For custom types (e.g., ID)
        default:
            push_i(type->data.simpleType.type);
            break;
        }
        store(id);
        break;
    case TYPE_ARRAY:
        push_i(9);
        store(id);
        push_i(type->data.arrayType.length);
        astore(id);

        translate_variable(mystrcat(id, "_0"), type->data.arrayType.elementType);
        break;
    default:
        break;
    }

    // push the type to the top of the stack
    // Store the variable type 
}
int translate_variable(char* id, Type * type) {
    // TODO : code to find out the type of the identifier based on types in C
    //TODO: some table to match 0 to int for example 1 long... 
    put_label_var(id, 0, 16);// 2 bytes for the value, the 0 is a dynamic thing for type
  
    translate_type(id, type);
  
    return 0;
}
int translate_var_declarations(Node* localVars) {
    while (localVars != NULL) {
        char** currentId = localVars->data->Ids;

        for (int i = 0; i < localVars->data->cntId;i++) { // Loop until we hit a NULL pointer
            char* suffix = mystrcat("_",remove_last_three_chars( localVars->fileName));
            translate_variable(mystrcat( * currentId, suffix) , localVars->data->type);
            currentId++; // Move to the next ID
         
        }
            if (localVars->next != NULL) {
                localVars = localVars->next;
            }
            else {
                break;
            }
    }
}

