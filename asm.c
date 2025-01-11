#include "asm.h"
#include "ControlGraph.h"
#include "parser.h"



#define asm_code_header "[section code_ram]\n\tjump start\n"
#define asm_data_header "[section data_ram]\n"
#define asm_footer "halt:\n\thlt\n"

#define mnemonic_1(mnemonic, arg1) fprintf(asmCodeOut, "\t%s %s\n", mnemonic, arg1);
#define mnemonic_2(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %s, %s\n", mnemonic, arg1, arg2);
#define mnemonic_2_i(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %d, %s\n", mnemonic, arg1, arg2);
#define mnemonic_3(mnemonic, arg1, arg2, arg3) fprintf(asmCodeOut, "\t%s %s, %s, %s\n", mnemonic, arg1, arg2, arg3);
#define mnemonic_0(mnemonic) fprintf(asmCodeOut, "\t%s\n", mnemonic);

#define push(value) mnemonic_1("push", value)
#define pop() mnemonic_0("pop")
#define iadd() mnemonic_0("iadd")
#define ladd(op1, op2, to) mnemonic_0("ladd")

#define jump(target) mnemonic_1("jump", target)
#define jz(target, cond) mnemonic_2("jz", target, cond)



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
#define put_comment_var(comment) fprintf(asmDataOut, "\t\t;%s\n", comment);
#define put_label_var(name, type, value) \
    fprintf(asmDataOut, "%s:\n", name); \
    fprintf(asmDataOut, "    .type: 0x%x ; Offset for `type`\n", type); \
    fprintf(asmDataOut, "    .value: 0x%x ; Offset for `value`\n", value);

int translate_variable(Var var) {
    // TODO : code to find out the type of the identifier based on types in C
    //TODO: some table to match 0 to int for example 1 long... 
    put_label_var(var.id, 0, 4);// 4 bytes for the value, the 0 is a dynamic thing for type


  
    return 0;
}


