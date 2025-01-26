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
#define wide_add() mnemonic_0("wide_add")
#define wide_sub() mnemonic_0("wide_sub")
#define wide_mult() mnemonic_0("wide_mult")
#define wide_div() mnemonic_0("wide_div")
#define ladd(op1, op2, to) mnemonic_0("ladd")

#define jump(target) mnemonic_1("jump", target)
#define jz(target) mnemonic_1("jz", target)
#define jgt(target) mnemonic_1("jgt", target)
#define jge(target) mnemonic_1("jge", target)
#define jlt(target) mnemonic_1("jlt", target)
#define jle(target) mnemonic_1("jle", target)


#define store(value) mnemonic_1("store", value)
#define astore(value) mnemonic_1("astore", value)
#define wide_store() mnemonic_0("wide_store")

#define store_label_value(value) fprintf(asmCodeOut,"\tstore %s.value\n", value)
#define store_label_type(type) fprintf(asmCodeOut,"\tstore %s.type\n", type)



char* labelName() {
	char* c = malloc(sizeof(char) * 32);
	sprintf(c, "label_%d", labelCounter++);
	//c= mystrcat(c, ".type");
	return c;
}


FILE* asmCodeOut;
FILE* asmDataOut;
int labelCounter = 0;

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
int generateAsm(Node * localVars) {
    fprintf(asmCodeOut, asm_code_header);
    //fprintf(asmDataOut, asm_data_header);
    translate_var_declarations(localVars);
    fprintf(asmDataOut, asm_footer);

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
void check_type(char* operand,char * fileName) {
	// CHECK TYPE OF OPERAND
					// true, false -> bool
					// number in C -> number 
					// starts with "" -> string
					// starts with '' -> char
					// Bits
	  // Determine the type of operand
	//TODO: load,  wide add, wide sub.. , how to differentiate adds, .


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
		push(lab);
		// Handle string type
	}
	else if (operand[0] == '\'' && operand[strlen(operand) - 1] == '\'') {
		printf("Type: char\n");
		char* lab = labelName();
		labelCnt++;
		put_label_string(lab, TYPE_CHAR, operand);
		push(lab);
		// Handle char type
	}
	else if (strcmp(operand, "true") == 0 || strcmp(operand, "false") == 0) {
		printf("Type: bool\n");
		char* lab = labelName();
		labelCnt++;
		put_label_string(lab , TYPE_BOOL, operand);
		push(lab);
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
	
			put_label_literal(lab, TYPE_INT, operand);
			push(lab);
			// Handle number type
		}
		else {
			printf("Type: variable\n");
			char* suffix = mystrcat("_", remove_last_three_chars(fileName));

			push(mystrcat(operand, suffix));
			/*char* suffix = mystrcat("_", remove_last_three_chars(fileName));
			push(mystrcat(operand, suffix));*/
			// a variable should have a push for a label with its name and file name only
			// Handle variable type
			// THe var definition from multiple 
		}
	}
	return;
}
int translateOT(OTNode* tree, char * fileName) {
	
		switch (tree->type)
		{
		case NODE_TYPE_OPERAND:
			check_type(tree->value.operand, fileName);

			break;
		case NODE_TYPE_OPERATOR:
			/////////////////////
			//////////////////
			// based on the 2 top values of the stack, we will have a wide_add, wide_sub...
			// inside those wide functions I will check the types
			// and jump to different implementations of add, sub 
			/////////
			for (int i = 0; i < tree->cntOperands; i++) {
				translateOT(tree->operands[i], fileName);
			}
			if (strcmp(tree->value.operator, "<") == 0) {
				printf("<");
				
				break;
			}
			if (strcmp(tree->value.operator, ">") == 0) {
				printf(">");
				
			}
			if (strcmp(tree->value.operator, "=") == 0) {
				wide_store();

				printf("=");
			}
			if (strcmp(tree->value.operator, "<=") == 0) {
				printf("<=");
			}
			if (strcmp(tree->value.operator, ">=") == 0) {
				printf(">=");
			}
			if (strcmp(tree->value.operator, "==") == 0) {
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
			}
			if (strcmp(tree->value.operator, "+") == 0) {
				wide_add();
				char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);
				printf("+");
			}

			if (strcmp(tree->value.operator, "-") == 0) {
				wide_sub();
				char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);
				printf("-");
			}
			if (strcmp(tree->value.operator, "*") == 0) {
				wide_mult();
				char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);
				printf("*");
			}
			if (strcmp(tree->value.operator, "/") == 0) {
				wide_div();
				char* lab = labelName();
				put_label_var(lab, 0, 0);
				store_label_type(lab);
				store_label_value(lab);
				push(lab);
				printf("/");
			}
			break;

		default:
			break;
		}
	
}
int translateInstructions(controlFlowGraphBlock * node, char* fileName) {
	if (node->instructions)
	{
		for (int i = 0; i < (node->instructions->size); i++) {
			cfgBlockContent* instruction = node->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_OTNODE) {
				translateOT(instruction->ot, fileName);
			}
			
		}
	}
	return 0;
}





int translate(Subroutine** subroutines, int cnt, char* fileName) {
	for (int i = 0; i < cnt; i++) {
		 translateCfg(subroutines[i]->cfg, NULL, fileName);
	}
	//jump("halt");

	return 0;
}

int translateCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start , char* fileName) {

	translateInstructions(node, fileName);

	put_comment("if")
	//TODO: type check that the condition is an expression that returns the type bool
	char* goThrough = labelName();
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
	/*	if (translate_statement(ifs.statement.statement, table, lastGoThroughLabel) != 0) {
			return 1;
			//TODO: understand this condition
		}*/
	controlFlowGraphBlock* ThenNode = node->nodes[0];
	translateCfg(ThenNode, node, fileName);
	if (isElseNode==1) {
		jump(goThrough)
			put_label(elseLabel)
			put_comment("else")
			/*if (translate_statement(ifs.elseStatement, table, lastGoThroughLabel) != 0) {
				return 1;
			}*/
			controlFlowGraphBlock* ElseNode = node->nodes[1];
		translateCfg(ElseNode, node, fileName);
		jump(goThrough)

	}
	put_comment("endif")
		put_label(goThrough)



	return 0;
}
int translateCfgWhileStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start , char* fileName) {
	   
	translateInstructions(node, fileName);

	controlFlowGraphBlock* whilebody = node->nodes[0];
	  translateCfg(whilebody, node , fileName);
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
			cfg->drawn++;
		}
		if (cfg->blocktype == IfExitBlock && cfg->drawn > 1) {
			break;
		}

		if (cfg->outNodeCount <= 0) {
			   

			return 0;
		}
		  translateCfgBaseStatement(cfg, start , fileName);
		break;
	}
	return 0;

}
