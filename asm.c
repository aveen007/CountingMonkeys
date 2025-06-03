#include "asm.h"
#include "ControlGraph.h"
#include "parser.h"
#include "localVariable.h"



#define asm_code_header "[section ram, code]\n\tjump start\n"
#define asm_data_header "data:\n"
#define asm_footer "halt:\n\thlt\n"

#define mnemonic_1(mnemonic, arg1) fprintf(asmCodeOut, "\t%s %s\n", mnemonic, arg1);
#define mnemonic_1_i(mnemonic, arg1) fprintf(asmCodeOut, "\t%s %d\n", mnemonic, arg1);
#define mnemonic_2(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %s, %s\n", mnemonic, arg1, arg2);
#define mnemonic_2_i(mnemonic, arg1, arg2) fprintf(asmCodeOut, "\t%s %d, %s\n", mnemonic, arg1, arg2);
#define mnemonic_3(mnemonic, arg1, arg2, arg3) fprintf(asmCodeOut, "\t%s %s, %s, %s\n", mnemonic, arg1, arg2, arg3);
#define mnemonic_0(mnemonic) fprintf(asmCodeOut, "\t%s\n", mnemonic);
//instruction push_sf(value) "{1}";
//instruction pop_sf();
//instruction init(target);
//instruction call(ptr) "{1}";
//instruction ret();

#define push_sf() mnemonic_0("push_sf")
#define pop_sf() mnemonic_0("pop_sf")

#define call(value) mnemonic_1("call", value)
#define ret() mnemonic_0("ret")
#define load() mnemonic_0("load")



#define push(value) mnemonic_1("push", value)
#define push_i(value) mnemonic_1_i("push", value)
#define write() mnemonic_0("write")
#define read() mnemonic_0("read")
#define pop() mnemonic_0("pop")
#define iadd() mnemonic_0("iadd")
#define wide_add() mnemonic_0("wide_add")
#define wide_sub() mnemonic_0("wide_sub")
#define wide_mult() mnemonic_0("wide_mult")
#define wide_div() mnemonic_0("wide_div")
#define wide_mod() mnemonic_0("wide_mod")
#define ladd(op1, op2, to) mnemonic_0("ladd")

#define jump(target) mnemonic_1("jump", target)
#define jz(target) mnemonic_1("jz", target)
#define jgt(target) mnemonic_1("jgt", target)
#define jge(target) mnemonic_1("jge", target)
#define jlt(target) mnemonic_1("jlt", target)
#define jle(target) mnemonic_1("jle", target)
#define jmp_emp(target) mnemonic_1("jmp_emp", target)


#define store(value) mnemonic_1("store", value)
#define r_st(value) mnemonic_1("r_st %d", value)
#define astore(value) mnemonic_1("astore %d", value)
#define wide_store() mnemonic_0("wide_store")

#define store_label_value(value) fprintf(asmCodeOut,"\tstore %s.value\n", value)
#define store_label_type(type) fprintf(asmCodeOut,"\tstore %s.type\n", type)


#define add_s(offset) mnemonic_1_i("add_s", offset)
#define sub_s(offset) mnemonic_1_i("sub_s", offset)

char* labelName() {
	char* c = malloc(sizeof(char) * 32);
	sprintf(c, "label_%d", labelCounter++);
	//c= mystrcat(c, ".type");
	return c;
}
char* labelNameOffsetted( int offset) {
	char* c = malloc(sizeof(char) * 32);
	sprintf(c, "label_%d", offset);
	//c= mystrcat(c, ".type");
	return c;
}


FILE* asmCodeOut;
FILE* asmDataOut;
int labelCounter = 0;

FunctionVariables* curFuncVars;
FunctionVariables** fileFuncs;
int cntFuncs;
char* labelNameIdent(char* ident) {
    char* c = malloc(sizeof(char) * 32);
    sprintf(c, "%s_%d", ident, labelCounter++);
    return c;
}

#define put_label(name) fprintf(asmCodeOut, "%s:\n", name);
#define put_comment(comment) fprintf(asmCodeOut, "\t\t;%s\n", comment);
#define put_comment_var(comment) fprintf(asmCodeOut, "\t\t;%s\n", comment);
#define put_label_var(name, type, value) \
    fprintf(asmDataOut, "%s: \n", name); \
    fprintf(asmDataOut, "    .type: dd 0x%x ; Offset for `type`\n", type); \
    fprintf(asmDataOut, "    .value: dd 0x%x ; Offset for `value`\n", value);
#define put_label_literal(name, type, value) \
    fprintf(asmDataOut, "%s:  \n", name); \
    fprintf(asmDataOut, "    .type: dd 0x%x ; Offset for `type`\n", type); \
    fprintf(asmDataOut, "    .value: dd %s ; Offset for `value`\n", value);
#define put_label_string(name, type, value) \
    fprintf(asmDataOut, "%s: \n", name); \
    fprintf(asmDataOut, "    .type: dd 0x%x ; Offset for `type`\n", type); \
    fprintf(asmDataOut, "    .value: dd '%s' ; Offset for `value`\n", value);
int labelCnt = 0;
int generateAsm(FunctionVariables*** funVars) {


}
void translate_type(Type* type) {
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
        //store(id);
        break;
    case TYPE_ARRAY:
        push_i(9);
       // store(id);
        push_i(type->data.arrayType.length);
       // astore(id);
		//TODO: see what I would do about arrays
       // translate_variable(mystrcat(id, "_0"), type->data.arrayType.elementType);
        break;
    default:
        break;
    }

    // push the type to the top of the stack
    // Store the variable type 
}
int translate_variable(char* id, Type * type) {
    put_label_var(id, 0, 16);// 2 bytes for the value, the 0 is a dynamic thing for type
  
    translate_type(id, type);
  
    return 0;
}

void check_type(char* operand,char * fileName, int isAssignment) {
	// CHECK TYPE OF OPERAND
					// true, false -> bool
					// number in C -> number 
					// starts with "" -> string
					// starts with '' -> char
					// Bits
	  // Determine the type of operand


	/// <summary>
	/// after that I will push 2 values for literals which are the payload and the type and as for the vars
	/// I will load the var which within its assembly implementation pushes the payload and the type
	/// </summary>
	/// <param name="operand"></param>
	if (operand[0] == '"' && operand[strlen(operand) - 1] == '"') {
		printf("Type: string\n");
		char* lab = labelName();
		labelCnt++;
		int h = strlen(operand);
		char* temp=malloc(h);
		sscanf(operand, "\"%[^\"]\"", temp); // Extract everything between the double quotes

		// Surround the string with single quotes
		put_label_string(lab, TYPE_STRING, temp);
		push(operand)
		push(TYPE_STRING)
		//push(lab);
		// Handle string type
	}
	else if (operand[0] == '\'' && operand[strlen(operand) - 1] == '\'') {
		printf("Type: char\n");
		char* lab = labelName();
		labelCnt++;
		put_label_string(lab, TYPE_CHAR, operand);
		push(operand)
		push(TYPE_CHAR)
		// push(lab);
		// Handle char type
	}
	else if (strcmp(operand, "true") == 0 || strcmp(operand, "false") == 0) {
		printf("Type: bool\n");
		char* lab = labelName();
		labelCnt++;
		put_label_string(lab , TYPE_BOOL, operand);
		push(operand)
		push(TYPE_BOOL)
		//push(lab);
		// Handle boolean type
	}
	else {
		int isNumber = 1;
		for (size_t i = 0; i < strlen(operand); i++) {
			if (!isdigit(operand[i]) && operand[i] != '.') {
				isNumber = 0;
				break;
			}
		}

		if (isNumber) {
			printf("Type: number\n");
			char* lab = labelName();
			labelCnt++;
	
		//	put_label_literal(lab, TYPE_INT, operand);
			push(operand)
			push_i(TYPE_INT)
			//push(lab);
			// Handle number type
		}
		else {
			printf("Type: variable\n");
			char* suffix = mystrcat("_", remove_last_three_chars(fileName));
			//load the variable from ram
			// push it and it's type
		
			int offset = findVar(operand);

			if (offset >= 0) {
				add_s(offset)
			}
			else {
				sub_s(-1*offset)
			}
			if (isAssignment == 0) {
		load()// so load takes the pointer from the top of the stack and pushes the 
			// type and value of the var
			// so if it's not assignement we want those values and we want to pop
			// the pointer
			//if it is though, no we want to know where to store to 
			}


		}
	}
	return;
}
int findVar(char * operand) {
	//int offset;
	VarNode* curr;
	
	curr = curFuncVars->localVariables;
	for (int i = 0; i < curFuncVars->cntVars; i++) {
		if (strcmp(curr->id, operand) == 0)
		{

		return curr->offset;

		}
		curr = curr->next;

	}
	curr = curFuncVars->parameters;
	for (int i = 0; i < curFuncVars->cntArgs; i++) {
		if (strcmp(curr->id, operand) == 0)
		{

			return -1*curr->offset;

		}
		curr = curr->next;

	}
	return 0;
}
FunctionVariables * findFunc(char* operand) {
	//int offset;
	FunctionVariables* curr;

	for (int i = 0; i < cntFuncs; i++) {
		curr = fileFuncs[i];
		if (strcmp(curr->funcName, operand) == 0)
		{

			return curr;

		}

	}
	
}
int translateOT(OTNode* tree, char * fileName, int isAssignement) {
	
		switch (tree->type)
		{
		case NODE_TYPE_OPERAND:
			check_type(tree->value.operand, fileName, isAssignement);

			break;
		case NODE_TYPE_OPERATOR:
			/////////////////////
			//////////////////
			// based on the 2 top values of the stack, we will have a wide_add, wide_sub...
			// inside those wide functions I will check the types
			// and jump to different implementations of add, sub 
			/////////
			
		
			if (strcmp(tree->value.operator, "!=") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, isAssignement);
				}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jz(label1)
				push("1"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
				push("0");//true branch
				put_comment("true branch");

				put_label(label2)


					free(label1);
				free(label2);
			}
			else if (strcmp(tree->value.operator, "<") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, isAssignement);
				}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jlt(label1);
				push("0"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
					push("1");//true branch
				put_comment("true branch");

				put_label(label2)
					free(label1);
				free(label2);
				printf("<");
				
			}
			else if (strcmp(tree->value.operator, ">") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, isAssignement);
				}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jgt(label1);
				push("0"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
					push("1");//true branch
				put_comment("true branch");

				put_label(label2)
					free(label1);
				free(label2);
				printf(">");

				
			}
			else if (strcmp(tree->value.operator, "=") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, 1);
				}
				isAssignement = 0;
			    // find offset of the var
				
				wide_store();
				printf("=");
				break;

			}
			else if (strcmp(tree->value.operator, "<=") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, isAssignement);
				}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jle(label1);
				push("0"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
					push("1");//true branch
				put_comment("true branch");

				put_label(label2)
					free(label1);
				free(label2);
				printf("<=");
				break;

			}
			else if (strcmp(tree->value.operator, ">=") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, 0);
				}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jge(label1);
				push("0"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
					push("1");//true branch
				put_comment("true branch");

				put_label(label2)
					free(label1);
				free(label2);
				printf(">=");
				break;

			}
			else if (strcmp(tree->value.operator, "==") == 0) {
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName, 0);
			}
				wide_sub();
				pop();// because sub pushes the type first
				char* label1 = labelName();
				char* label2 = labelName();
				jz(label1);
				push("0"); //false branch
				put_comment("false branch");
				jump(label2);
				put_label(label1)
				push("1");//true branch
				put_comment("true branch");

				put_label(label2)
				free(label1);
				free(label2);

				printf("==");
				break;

			}
			else if (strcmp(tree->value.operator, "+") == 0) {
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName, 0);
			}
				wide_add();
				/*char* lab = labelName();
				put_label_var(lab, 0, 0);*/
			/*	store_label_type(lab);
				store_label_value(lab);*/
				
				//push(lab);
				printf("+");
			}

			else if (strcmp(tree->value.operator, "-") == 0) {
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName, 0);
			}
				wide_sub();
			/*	char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);*/
				printf("-");
				break;

			}
			else  if (strcmp(tree->value.operator, "*") == 0) {
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName ,0);
			}
				wide_mult();
				/*char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);*/
				printf("*");
				break;

			}
			else if (strcmp(tree->value.operator, "/") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, 0);
				}
				wide_div();
				/*char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);*/
				printf("/");
				break;
			}
			else if (strcmp(tree->value.operator, "%") == 0) {
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName, 0);
			}
			wide_mod();
		/*	char* lab = labelName();
			put_label_var(lab, 0, 0);
			store_label_type(lab);
			store_label_value(lab);
			push(lab);*/
			printf("%");
			break;
			}
			else {

			printf("indexer");
			if (strcmp(tree->value.operator,"print") == 0 ){
				for (int i = 0; i < tree->cntOperands; i++) {

					translateOT(tree->operands[i], fileName, 0);
					
				}
			
				pop()
				write();

			}
			else if (strcmp(tree->value.operator,"scan") == 0) {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, 0);
				}
				read();
			}
			else if (strcmp(tree->value.operator,"=.") == 0) {
			//TODO: here I will read from field
			}
			else if (strcmp(tree->value.operator,".=") == 0) {
				//TODO: here I will write in field
			}
			else {
				for (int i = 0; i < tree->cntOperands; i++) {
					translateOT(tree->operands[i], fileName, 0);
				}
				// function call
				//char* returnLabel = labelName();
			/*	push(returnLabel);
				store("ret");*/
				//jump(tree->value.operator);
		/*		put_label(returnLabel);
				push("halt");
				store("ret");*/
				FunctionVariables* func = findFunc(tree->value.operator);
				for (int i = 0; i < func->cntArgs; i++) {
					pop_sf();//here I am just saving a place for the called function's args in the SF of

					pop_sf(); //I am poping for the type and value					// the caller function 
				}
				call(tree->value.operator);
				char* lab = labelName();
				//I don't remember what this code was for?
				// yeah so I remember, this is so that I save the value that I returned in some label and push the label
				// but I don't need that cuz I already pushed the value that I want
				//put_label_var(lab, 2, 16);// 2 bytes for the value, the 0 is a dynamic thing for type
				//push(lab);
				//wide_store()
				//push(lab);

			}

			}
			break;

		default:
			// indexer
			break;
		}
	
}
int translateInstructions(controlFlowGraphBlock * node, char* fileName) {
	if (node->instructions)
	{
		for (int i = 0; i < (node->instructions->size); i++) {
			cfgBlockContent* instruction = node->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_OTNODE) {
				translateOT(instruction->ot, fileName, 0);
			}
			
		}
	}
	return 0;
}



int translate(Subroutine** subroutines, FunctionVariables ** funcVars,int cnt, char* fileName) {

	//TODO: here I am translating classes
	//
	//
	



	//here I am translating functions
	for (int i = 0; i < cnt; i++) {
		curFuncVars = funcVars[i];
		fileFuncs = funcVars;
		cntFuncs = cnt;
		char* suffix = mystrcat("_", remove_last_three_chars(fileName));
		put_label(subroutines[i]->name);
		
			int size_args = funcVars[i]->cntArgs;// added one for old_ret
			if (funcVars[i]->parameters) {

			for (int i = 0; i < size_args; i++) {
				
				sub_s(i + 1)
				wide_store()
			}
			}
			if (funcVars[i]->localVariables) {

			for (int j = 0; j < funcVars[i]->localVariables->cntVars; j++) {
	

				pop_sf()
				pop_sf()
			}
			VarNode* locals = funcVars[i]->localVariables;
			for (int j = 0; j < funcVars[i]->cntVars;j++) {

			//HERE : I am initializing the type 
				// create a label with the type of the variable and value zero
				// push the label
				// sub_s the offset of the var
				// wide_store the initialized value 
				/*char* lab = labelName();*/
				/*translate_variable(lab, locals->type);*/
				//push(lab);
				//Everywhere else I push value and then push type
				push_i(0)//some randome init value 
				translate_type(locals->type);
				add_s(locals->offset)
					wide_store()
					locals = locals->next;

			}

			}
		
		translateCfg(subroutines[i]->cfg, NULL, fileName);
		if (strcmp(subroutines[i]->name, "main") != 0) {
			//here I want to take the last variable which I put on top of the stack and put it inside the return label and push that to the stack instead
			//char* lab = labelName();
			//put_label_var(lab, 2, 16);// 2 bytes for the value, the 0 is a dynamic thing for type

			//push(lab);
			//wide_store()
			//push(lab)
			ret()

		}
		/*jmp_emp("ret.type");*/
	}
	//jump("halt");

	return 0;
}
int goThroughLabelCount=0;

int translateCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start , char* fileName) {

	translateInstructions(node, fileName);

	put_comment("if")
		//TODO: type check that the condition is an expression that returns the type bool
	goThroughLabelCount = labelCounter;
	char* goThrough = labelName();
		//print(goThroughLabelCount);

	char* elseLabel="";
		
	int isElseNode = node->outNodeCount > 1 ? 1 : 0;
	if (isElseNode == 1) {
		elseLabel = labelName();
		jz( elseLabel) // if r0 == 0 goto else conditional statement
		
	}
	else {
		jz(goThrough) // if r0 == 0 goto else conditional statement

	}
	
	put_comment("then")

	controlFlowGraphBlock* ThenNode = node->nodes[0];
	translateCfg(ThenNode, node, fileName);
	if (isElseNode==1) {
		//jump(goThrough)//prbbly not needed //TODO check
			put_label(elseLabel)
			put_comment("else")
		
			controlFlowGraphBlock* ElseNode = node->nodes[1];
		translateCfg(ElseNode, node, fileName);
		//jump(goThrough)

	}

	put_comment("endif")

		//put_label(goThrough)




	return 0;
}
int translateCfgWhileStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start , char* fileName) {
	   

	put_comment("while")
		char* loopbackLabel = labelName();
	put_label(loopbackLabel)
	translateInstructions(node, fileName);
	
	char* goThrough = labelName();
		jz(goThrough) // if r0 == 0 goto else conditional statement
		put_comment("while body")
			//TODO: type check the condition

	controlFlowGraphBlock* whilebody = node->nodes[0];
	translateCfg(whilebody, node , fileName);
	jump(loopbackLabel)
		put_label(goThrough)
		put_comment("end while")

	  controlFlowGraphBlock* exitWhilebody = node->nodes[1];
	  translateCfg(exitWhilebody, node , fileName);
	return 0;
}
int translateCfgBaseStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start , char* fileName) {
	   
	translateInstructions(node, fileName);

	  translateCfg(node->nodes[0], node , fileName);
	return 0;

}

int translateCfg(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start , char* fileName) {
	switch (cfg->blocktype)
	{
	case IfBlock:
		  translateCfgIfStatement(cfg, start , fileName);
		break;
	case WhileBlock:
		if (cfg->drawn == 0) {
			cfg->drawn = 1;
			  translateCfgWhileStatement(cfg, start , fileName);
		}
		else if (cfg->drawn == 1) {
			cfg->drawn = 2;
			return 0;
		}
		else {

			return 0;
		}
		break;
	case BreakBlock:
		   
		translateInstructions(cfg, fileName);

		return 0;
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {
			   
			translateInstructions(cfg, fileName);

			return 0;
		}
		  translateCfgBaseStatement(cfg, start , fileName);
		break;
	default:
		if (cfg->blocktype == IfExitBlock) {
			char* goThrough = labelNameOffsetted(goThroughLabelCount);
			jump(goThrough)
			put_label(goThrough);
			cfg->drawn++;
		}

		if (cfg->outNodeCount <= 0) {
			   

			return 0;
		}
		  translateCfgBaseStatement(cfg, start , fileName);
		break;
	}
	return 0;

}
