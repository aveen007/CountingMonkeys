#include "ControlGraph.h"
#include "parser.h"
#include <stdlib.h>
#define allocator_may_return_null 1 

#pragma region  definitions
controlFlowGraphBlock* current;
callGraph* currentCG;
callGraph** CG;
int procedures;
ErrorInfoCFG* errors;
controlFlowGraphBlock** cfgs;

#pragma endregion

#pragma region cfg errors and file info


CfgsInfo* createCfgsInfo(ParseTree* ast) {
	CfgsInfo* info = malloc(sizeof(CfgsInfo));
	info->cfgs = createCfgs(ast);
	info->errors = NULL;

	return info;
}
controlFlowGraphBlock** createCfgs(ParseTree* ast) {
	controlFlowGraphBlock** cfgs = (controlFlowGraphBlock**)malloc(sizeof(controlFlowGraphBlock*) * ast->childrenCount);

	procedures = 0;

	return cfgs;
}

ErrorInfoCFG* createErrorInfoNodeCFG(const char* message, int line, int position) {
	ErrorInfoCFG* newNode = (ErrorInfoCFG*)malloc(sizeof(ErrorInfoCFG));
	if (newNode == NULL) {
		// Handle memory allocation failure
		perror("Failed to allocate memory for ErrorInfo");
		exit(EXIT_FAILURE);
	}
	newNode->message = _strdup(message); // Duplicate the message string
	newNode->line = line;
	newNode->position = position;
	newNode->next = NULL;
	return newNode;
}
// Function to free the ErrorInfo linked list
void freeErrorInfoCFG(ErrorInfoCFG* head) {
	while (head != NULL) {
		ErrorInfoCFG* temp = head;
		head = head->next;
		free(temp->message); // Free the string
		free(temp);          // Free the node
	}
}
void addErrorCFG(ErrorInfoCFG** errors, ErrorInfoCFG* newError) {

	if (*errors == NULL) {
		// If the list is empty, add the new error as the head
		*errors = newError;
	}
	else {
		// Otherwise, append the new error at the end
		ErrorInfoCFG* current = *errors;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newError; // Add the new error to the end
	}

	return;
}
#pragma endregion

# pragma region subprogram detail construction
Subroutine** createSubroutines(ParseTree* ast) {
	Subroutine** subroutines = (Subroutine**)malloc(sizeof(Subroutine*) * ast->childrenCount);
	for (int i = 0; i < ast->childrenCount; i++) {
		subroutines[i] = malloc(sizeof(Subroutine));
	}
	return subroutines;
}
Subroutine** DefineSubprogram(char* fileName, controlFlowGraphBlock** cfgs, ParseTree* tree) {


	if (tree) {
		// get the funcDefs, attach each one to a subroutine and get for each subroutine the required details
		  // construct the proper cfg for each subroutine
		tree = tree->children[0];

		Subroutine** subprograms = createSubroutines(tree);
		for (int i = 0; i < tree->childrenCount; i++) {
			ParseTree* funcSigNode = tree->children[i]->children[0]->children[0];
			if (funcSigNode) {

				char* name = funcSigNode->token;
				Position* pos = (Position*)malloc(sizeof(Position));
				pos->line = funcSigNode->line;
				pos->width = funcSigNode->position;
				subprograms[i]->signatureDetails = (SignatureDetails*)malloc(sizeof(SignatureDetails));
				if (tree->children[i]->children[0]->childrenCount > 1) {
					subprograms[i]->signatureDetails->returnType = HandleType(tree->children[i]->children[0]->children[1]);
				}
				//list arg defs
				if (funcSigNode->childrenCount != 0) {
					ParseTree* argsTree = funcSigNode->children[0];
					if (argsTree) {
						subprograms[i]->signatureDetails->arguments = (ArgumentDef**)malloc(sizeof(ArgumentDef*) * argsTree->childrenCount);
						for (int j = 0; j < argsTree->childrenCount; j++) {
							subprograms[i]->signatureDetails->arguments[j] = (ArgumentDef*)malloc(sizeof(ArgumentDef));
							subprograms[i]->signatureDetails->arguments[j]->name = argsTree->children[j]->children[0]->token;
							if (argsTree->childrenCount > 1) {

								subprograms[i]->signatureDetails->arguments[j]->type = HandleType(argsTree->children[1]);
							}

							///Questions
							//Payload

						}
					}
				}
				subprograms[i]->signatureDetails->position = pos;
				subprograms[i]->name = name;
				subprograms[i]->cfg = cfgs[i];
			}
		}
		return subprograms;
	}
	return;

}

#pragma endregion

#pragma region  Types


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

	if (typeNode->children[0]->childrenCount != 0) {
		int size = typeNode->children[0]->children[1]->childrenCount;
		return create_array_type(HandleType(typeNode->children[0]->children[0]), size);
	}
	else {
		char* typeName = typeNode->children[0]->token;

		if (strcmp(typeName, "int") == 0) {
			return create_simple_type(TYPE_INT, "");

		}
		if (strcmp(typeName, "bool") == 0) {
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
#pragma endregion

# pragma region operations tree



// Function to create a new node for an operator
OTNode* createOperatorNode(char* operator) {
	OTNode* newNode = (OTNode*)malloc(sizeof(OTNode));
	newNode->type = NODE_TYPE_OPERATOR;
	newNode->value.operator = operator;
	newNode->operands = NULL;
	newNode->cntOperands = 0;

	return newNode;
}

// Function to create a new node for an operand
OTNode* createOperandNode(char* operand) {
	OTNode* newNode = (OTNode*)malloc(sizeof(OTNode));
	newNode->type = NODE_TYPE_OPERAND;
	newNode->value.operand = operand;
	newNode->operands = NULL;
	newNode->cntOperands = 0;

	return newNode;
}

// Function to free the operations tree
void freeTree(OTNode* root) {
	if (root) {
		freeTree(root->operands);
		free(root);
	}
}


// Function to print the tree in a readable format (in-order traversal)
char* printTree(char* treeText, OTNode* node) {
	if (node == NULL) {
		return treeText;
	}

	if (node->type == NODE_TYPE_OPERATOR) {
		char* newText2 = mystrcat(treeText, " ");
		treeText = (char*)realloc(treeText, stringLen(newText2) * sizeof(char));
		treeText = newText2;
		char* newText3 = mystrcat(treeText, node->value.operator);
		treeText = (char*)realloc(treeText, stringLen(newText3) * sizeof(char));
		treeText = newText3;
		if (node->cntOperands != 0) {
			char* newText4 = mystrcat(treeText, "(");
			treeText = (char*)realloc(treeText, stringLen(newText4) * sizeof(char));
			treeText = newText4;

			char* newText40 = mystrcat(treeText, "\0");
			treeText = (char*)realloc(treeText, stringLen(newText40) * sizeof(char));
			treeText = newText40;
			for (int i = 0; i < node->cntOperands; i++) {
				treeText = printTree(treeText, node->operands[i]);
				if (i != node->cntOperands - 1) {
					char* newText5 = mystrcat(treeText, " , ");
					treeText = (char*)realloc(treeText, stringLen(newText5) * sizeof(char));
					treeText = newText5;
				}


			}
			char* newText6 = mystrcat(treeText, ")");
			treeText = (char*)realloc(treeText, stringLen(newText6) * sizeof(char));
			treeText = newText6;
		}
	}
	else {
		// It's an operand
		char* newText7 = mystrcat(treeText, (char*)node->value.operand);
		treeText = (char*)realloc(treeText, stringLen(newText7) * sizeof(char));
		treeText = newText7;
	}

	return treeText;
}

varDeclaration* CreateVarDeclaration(char** Ids,int cnt, Type* type) {
	varDeclaration* var = (varDeclaration*)malloc(sizeof(varDeclaration));
	var->Ids = Ids;
	var->type = type;
	var->cntId = cnt;
	return var;
}
OTNode* HandleOperationsTree(ParseTree* base) {
	OTNode* OT;
	if (strcmp(base->token, "CallOrIndexer") == 0) {

		OT = createOperatorNode(base->children[1]->token);

		insertCGToken(base->children[1]->token);

		base = base->children[0];
	}
	else {
		if (strcmp(base->token, "Unary")!=0) {

			if (base->childrenCount > 0) {
				OT = createOperatorNode(base->token);

			}
			else {
				OT = createOperandNode(base->token);
				return OT;
			}
		}
		else {
			OT = createOperatorNode(base->children[0]->token);
			OT->cntOperands = 1;
			OT->operands = malloc(sizeof(OTNode*) );

			OT->operands[0] = HandleOperationsTree(base->children[1]);
			return OT;
		}
	}
	OT->cntOperands = base->childrenCount;
	OT->operands = malloc(sizeof(OTNode*) * base->childrenCount);

	for (int i = 0; i < OT->cntOperands; i++) {

		OT->operands[i] = HandleOperationsTree(base->children[i]);
	}
	return OT;

}
void InsertInstruction(Instructions* instructions, ParseTree* ast) {
	if (strcmp(ast->token, "VarStatement") == 0) {
		instructions->size++;
		instructions->content = realloc(instructions->content, sizeof(cfgBlockContent*) * instructions->size);

		cfgBlockContent* content = createInstructionsVarStatement(ast);
		instructions->content[instructions->size - 1] = content;// Allocate memory for the new content
	}
	else if (strcmp(ast->token, "Expression") == 0) {
		instructions->size++;
		instructions->content = realloc(instructions->content, sizeof(cfgBlockContent*) * instructions->size);
		cfgBlockContent* content = createInstructionsExpression(ast);

		instructions->content[instructions->size - 1] = content;// Allocate memory for the new content
	}

}
Instructions* CreateInstructions() {

	Instructions* instructions = malloc(sizeof(Instructions));
	instructions->content = malloc(sizeof(cfgBlockContent*));
	instructions->size = 0;


	return instructions;
}



void insertCFGBlock(controlFlowGraphBlock* nodes, controlFlowGraphBlock* node) {

	nodes->outNodeCount++;
	nodes->nodes = (controlFlowGraphBlock**)realloc(nodes->nodes, (sizeof(controlFlowGraphBlock*) * nodes->outNodeCount));
	nodes->nodes[nodes->outNodeCount - 1] = node;

}
controlFlowGraphBlock* createCFGBlock(ParseTree* ast, BlockType blockType) {

	controlFlowGraphBlock* block = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
	block->nodes = (controlFlowGraphBlock**)malloc(sizeof(controlFlowGraphBlock*));
	block->outNodeCount = 0;
	block->ast = ast;
	block->instructions = CreateInstructions();
	block->blocktype = blockType;
	block->called = CreateCGNode(block);
	block->drawn = 0;
	return block;
}

#pragma endregion

#pragma region call graphs

void insertCG(controlFlowGraphBlock* caller, controlFlowGraphBlock* called) {
	caller->called->cnt++;
	caller->called->calledProcedures = realloc(caller->called->calledProcedures, sizeof(controlFlowGraphBlock*) * caller->called->cnt);
	caller->called->calledProcedures[caller->called->cnt - 1] = called;
}
void insertCGToken(char* called) {
	current->called->cntNames++;
	current->called->calledTokens = realloc(currentCG->calledTokens, sizeof(char*) * currentCG->cntNames);
	current->called->calledTokens[currentCG->cntNames - 1] = called;
}
callGraph* CreateCGNode(controlFlowGraphBlock* cfg) {
	callGraph* CGI = malloc(sizeof(callGraph));
	//CGI->node = cfg;
	CGI->calledProcedures = malloc(sizeof(controlFlowGraphBlock*));
	CGI->calledTokens = malloc(sizeof(char*));
	CGI->cnt = 0;
	CGI->cntNames = 0;
	return CGI;
}
controlFlowGraphBlock* FindCFG(controlFlowGraphBlock** cfgss, int cfgCnt, char* funcName) {
	controlFlowGraphBlock* found = NULL;
	for (int i = 0; i < cfgCnt; i++) {

		if (strcmp(cfgss[i]->ast->children[0]->children[0]->token, funcName) == 0)
		{
			found = cfgss[i];
			return found;
		}
	}
	return found;


}
cfgFile** HandleCallGraphs(cfgFile** allFiles, int fileCnt) {

	//here I will try to find for a given cfg from a file if it's called procedures do exist anywhere in any other file 
	for (int i = 0; i < fileCnt; i++) {
		for (int j = 0; j < allFiles[i]->ast->children[0]->childrenCount; j++) {
			for (int k = 0; k < allFiles[i]->cfgs->cfgs[j]->called->cntNames; k++) {
				int found = 0; // have we found the called proc
				controlFlowGraphBlock* calledFunc = NULL;
				for (int l = 0; l < fileCnt; l++) {


					calledFunc = FindCFG(allFiles[l]->cfgs->cfgs, allFiles[l]->ast->children[0]->childrenCount, allFiles[i]->cfgs->cfgs[j]->called->calledTokens[k]);
					if (calledFunc != NULL) {
						found = 1;
						break;
					}


				}
				if (found == 1) {
					insertCG(allFiles[i]->cfgs->cfgs[j], calledFunc);
				}
				else {
					//TODO: add line and posiion to called graph
					char* errorString;
					size_t errSize = (size_t)snprintf(NULL, 0, "call to undefined function : %s", allFiles[i]->cfgs->cfgs[j]->called->calledTokens[k]);
					errorString = malloc(errSize + 2);
					snprintf(errorString, errSize + 2, "call to undefined function : %s", allFiles[i]->cfgs->cfgs[j]->called->calledTokens[k]);
					ErrorInfoCFG* undefined = createErrorInfoNodeCFG(errorString, allFiles[i]->cfgs->cfgs[j]->ast->line, allFiles[i]->cfgs->cfgs[j]->ast->position);
					addErrorCFG(&allFiles[i]->cfgs->errors, undefined);
				}
			}
		}

	}
	return allFiles;
}

void printCallGraph(cfgFile** allFiles, int fileCnt) {
	char* fileName = "all";
	char* filename1;
	size_t filename_size1 = (size_t)snprintf(NULL, 0, "CG%s.dot", fileName);
	filename1 = malloc(filename_size1 + 2);
	snprintf(filename1, filename_size1 + 2, "CG.%s.dot", fileName);
	//printCallGraph(filename1);

	size_t pngFileSize1 = (size_t)snprintf(NULL, 0, "../cpoCompilerWin/%s.png", filename1);
	char* pngFilename1 = malloc(pngFileSize1 + 1); // Adjust size according to your needs
	snprintf(pngFilename1, pngFileSize1 + 1, "../cpoCompilerWin/%s.png", filename1);
	size_t makeTreeSize1 = (size_t)snprintf(NULL, 0, "dot -Tpng %s -o %s", filename1, pngFilename1);
	char* makeTreeGraph1 = malloc(makeTreeSize1 + 1); // Increase size for the command string
	snprintf(makeTreeGraph1, makeTreeSize1 + 1, "dot -Tpng %s -o %s", filename1, pngFilename1);
	FILE* myfile = fopen(filename1, "w");
	if (!myfile) {
		perror("Error opening file");
		return;
	}
	fprintf(myfile, "digraph G {\n");
	const char* graphSettings = "ordering=out;\n"
		"ranksep=.4;\n"
		"bgcolor=\"lightgrey\";  node [shape=box, fixedsize=false, fontsize=12, fontname=\"Helvetica-bold\", fontcolor=\"blue\"\n"
		"width=.25, height=.25, color=\"black\", fillcolor=\"white\", style=\"filled, solid, bold\"];\n"
		"\n"
		"edge [arrowsize=.5, color=\"black\", style=\"bold\"]\n";
	fprintf(myfile, "% s", graphSettings);
	for (int k = 0; k < fileCnt; k++) {

		// Write each control flow graph to the dot file

		for (int i = 0; i < allFiles[k]->ast->children[0]->childrenCount; i++) {

			fprintf(myfile, "    n%p ", allFiles[k]->cfgs->cfgs[i]);

			fprintf(myfile, "[label=\"%s\\n \"]\n", allFiles[k]->cfgs->cfgs[i]->ast->children[0]->children[0]->token);


			for (int j = 0; j < allFiles[k]->cfgs->cfgs[i]->called->cnt; j++) {

				fprintf(myfile, "    n%p -> n%p\n", allFiles[k]->cfgs->cfgs[i], allFiles[k]->cfgs->cfgs[i]->called->calledProcedures[j]);

			}

		}
	}
	fprintf(myfile, "}\n");

	system(makeTreeGraph1);

}
#pragma endregion


#pragma region Stack


// Function to create a stack
Stack* createStack() {
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	stack->capacity = 2; // Initial capacity
	stack->top = -1;
	stack->items = (ParseTree**)malloc(stack->capacity * sizeof(ParseTree*));
	return stack;
}

// Function to check if the stack is empty
int isEmpty(Stack* stack) {
	return stack->top == -1;
}

// Function to double the capacity of the stack
void resizeStack(Stack* stack) {
	stack->capacity *= 2;
	stack->items = (ParseTree**)realloc(stack->items, stack->capacity * sizeof(ParseTree*));
}

// Function to push an item onto the stack
void push(Stack* stack, ParseTree* block) {
	if (stack->top + 1 >= stack->capacity) {
		resizeStack(stack);
	}
	stack->items[++stack->top] = block;
}

// Function to pop an item from the stack
ParseTree* pop(Stack* stack) {
	if (isEmpty(stack)) {
		return NULL; // Stack is empty
	}
	return stack->items[stack->top--];
}

// Function to peek at the top item of the stack without removing it
ParseTree* peek(Stack* stack) {
	if (isEmpty(stack)) {
		return NULL; // Stack is empty
	}
	return stack->items[stack->top];
}

// Function to delete the stack and free its memory
void deleteStack(Stack* stack) {
	free(stack->items);
	free(stack);
}

#pragma endregion


#pragma region creating the CFG for each subprogram

CfgsInfo* CFGInterfacer(char* fileName, ParseTree* tree, int procedure) {
	if (tree) {
		tree = tree->children[0];
		CfgsInfo* cfgsInfo = createCfgsInfo(tree);
		cfgs = cfgsInfo->cfgs;

		for (int i = 0; i < tree->childrenCount; i++) {
			procedure++;
			procedures = procedure;
			controlFlowGraphBlock* cfg = createCFGBlock(tree->children[i], BaseBlock);
			// here I am just creating the start block
			cfgs[i] = cfg;
			current = cfg;
			currentCG = cfg->called;
			Stack* openNodes = createStack();
			push(openNodes, tree->children[i]);
			controlFlowGraphBlock*  end=ConstructCFG(openNodes, cfg, cfgsInfo->errors);
			controlFlowGraphBlock* cfgEnd = createCFGBlock(tree->children[i], BaseBlock);
			insertCFGBlock(end, cfgEnd);
			// print the result to a file
			 CreateFilePrint(fileName, cfgsInfo,cfgs[i]);

		}
		cfgsInfo->errors = errors;
		return cfgsInfo;
	}
	return;
}
void CreateFilePrint(char* fileName, CfgsInfo* info, controlFlowGraphBlock* cfg) {
	char* filename;
	size_t filename_size = (size_t)snprintf(NULL, 0, "%s.%s.dot", fileName, current->ast->children[0]->children[0]->token);
	filename = malloc(filename_size + 1);
	snprintf(filename, filename_size + 1, "%s.%s.dot", fileName, current->ast->children[0]->children[0]->token);
	CFGToDotFile(cfg, filename, info->errors);
	// Prepare output picture file name
	size_t pngFileSize = (size_t)snprintf(NULL, 0, "../cpoCompilerWin/%s.png", filename);
	char* pngFilename = malloc(pngFileSize + 1); // Adjust size according to your needs
	snprintf(pngFilename, pngFileSize + 1, "../cpoCompilerWin/%s.png", filename);
	size_t makeTreeSize = (size_t)snprintf(NULL, 0, "dot -Tpng %s -o %s", filename, pngFilename);
	char* makeTreeGraph = malloc(makeTreeSize + 1); // Increase size for the command string
	snprintf(makeTreeGraph, makeTreeSize + 1, "dot -Tpng %s -o %s", filename, pngFilename);
	system(makeTreeGraph);
}

cfgBlockContent* createInstructionsVarStatement(ParseTree* ast) {
	// add a var instruction
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));
	char** ids = (char*)malloc(sizeof(char*) * ast->children[0]->childrenCount);
	for (int k = 0; k < ast->children[0]->childrenCount; k++) {

		ids[k] = ast->children[0]->children[k]->token;
	}
	Type* instructionType = HandleType(ast->children[1]);
	varDeclaration* varDecl = CreateVarDeclaration(ids, ast->children[0]->childrenCount,instructionType);
	content->type = TYPE_VARDECLARATION;
	content->varDec = varDecl;  // Assign var declaration to the union
	return content;
}
cfgBlockContent* createInstructionsExpression(ParseTree* ast) {
	// add an operations tree
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));
	OTNode* otNode = HandleOperationsTree(ast->children[0]);
	content->type = TYPE_OTNODE;
	content->ot = otNode;  // Assign OTNode to the union
	return content;

}

int ImWhile = 0;
controlFlowGraphBlock* ConstructCFGIfStatement(Stack* openNodes, controlFlowGraphBlock * start) {
	ParseTree* tree = pop(openNodes);
	controlFlowGraphBlock* IfStatementCfg = createCFGBlock(tree, IfBlock);
	InsertInstruction(IfStatementCfg->instructions, tree->children[0]);

	int isElseNode = tree->childrenCount > 2 ? 1 : 0;
	// connect to father node
	ParseTree* Father = pop(openNodes);
	insertCFGBlock(start, IfStatementCfg);
	// make sure the father node is getting connected to this one 
	

	controlFlowGraphBlock* thenBlock = createCFGBlock(tree->children[1], ThenBlock);
	insertCFGBlock(IfStatementCfg, thenBlock);
	controlFlowGraphBlock* ifExitBlock;
	push(openNodes, tree->children[1]);
	controlFlowGraphBlock* finalOfThen = ConstructCFG(openNodes, thenBlock);

	if (isElseNode == 1) {
		// This is the "else" case
		controlFlowGraphBlock* elseBlock = createCFGBlock(tree->children[2], ElseBlock);
		insertCFGBlock(IfStatementCfg, elseBlock);
		push(openNodes, tree->children[2]);
		controlFlowGraphBlock* finalOfElse = ConstructCFG(openNodes, elseBlock);
		ifExitBlock = createCFGBlock(tree->children[2], IfExitBlock);
		insertCFGBlock(finalOfElse, ifExitBlock);
		//printf("blya");
	}
	else {

		ifExitBlock = createCFGBlock(tree->children[1], IfExitBlock);
	}

	insertCFGBlock(finalOfThen, ifExitBlock);
	push(openNodes, ifExitBlock);

	return ifExitBlock;

}
controlFlowGraphBlock* ConstructCFGWhileStatement(Stack* openNodes, controlFlowGraphBlock* start) {
	// initialize a new cfg node
	ParseTree* tree = pop(openNodes);
	ParseTree* Father = pop(openNodes);
	// I am going by the assumption that the openNodes always has on top the part of the tree that I want

	// and that the connection between the father and this node is done in the shared area so that his instructions are built, and now we are handeling his childeren only

	controlFlowGraphBlock* WhileBlockCfg = createCFGBlock(tree, WhileBlock);
	insertCFGBlock(start, WhileBlockCfg);
	InsertInstruction(WhileBlockCfg->instructions, tree->children[0]);
	controlFlowGraphBlock* whileBodyBlock = createCFGBlock(tree->children[1], WhileBodyBlock);
	for (int i = 1; i < tree->childrenCount; i++) {
	InsertInstruction(whileBodyBlock->instructions, tree->children[i]);
	}

	insertCFGBlock(WhileBlockCfg, whileBodyBlock);

	controlFlowGraphBlock* exitBlock = createCFGBlock(tree->children[1], WhileExitBlock);
	push(openNodes, tree->children[1]);
	controlFlowGraphBlock* finalOfWhileBody = ConstructCFG(openNodes, whileBodyBlock);
	insertCFGBlock(finalOfWhileBody, WhileBlockCfg);
	insertCFGBlock(WhileBlockCfg, exitBlock);
	ImWhile = 0;
	return exitBlock;
}
controlFlowGraphBlock* ConstructCFGBaseStatement(Stack* openNodes, controlFlowGraphBlock* start) {
	// when handeling weird blocks like exit blocks and so on

	ParseTree* tree = pop(openNodes);
	if (tree->childrenCount <= 0) {
		return start;
	}
	controlFlowGraphBlock* outBlock = start;
	for (int i = 0; i < tree->childrenCount; i++)
	{
		if (strcmp(tree->children[i]->token, "end") == 0)
			return outBlock;

		if (strcmp(tree->children[i]->token, "FuncSignature") == 0)
			continue;
		if (strcmp(tree->children[i]->token, "Expression") == 0|| strcmp(tree->children[i]->token, "VarStatement") == 0) {

			InsertInstruction(start->instructions, tree->children[i]);
			continue;
		}
		 // Assuming the node holds a pointer to a block
		push(openNodes, tree->children[i]);
		outBlock = ConstructCFG(openNodes, start);
		if (outBlock->blocktype == BreakBlock) {
			return outBlock;
		}
		if (outBlock != NULL) {
			start = outBlock;
		}

	}
	if (outBlock == NULL) {
		return start;
	}
	
	
	
	return outBlock;


}
controlFlowGraphBlock* ConstructCFG(Stack* openNodes, controlFlowGraphBlock* start) {
	// Print the current block
	if (isEmpty(openNodes)) { return; }
	ParseTree* tree = peek(openNodes);
	controlFlowGraphBlock* end;

	if (strcmp(tree->token, "IfStatement") == 0)
	{
		 end=ConstructCFGIfStatement(openNodes, start);
	}
	else if (strcmp(tree->token, "WhileStatement") == 0) {
		ImWhile = 1;
		end=ConstructCFGWhileStatement(openNodes, start);
	}
	else if (strcmp(tree->token, "BreakStatement") == 0) {
		if (ImWhile == 0) {
			ErrorInfoCFG* errorBreak = createErrorInfoNodeCFG("Break not in a loop", tree->line, tree->position);
			addErrorCFG(&errors, errorBreak);
		}
		pop(openNodes);
		controlFlowGraphBlock* breakBlock= createCFGBlock(tree, BreakBlock);;
			ParseTree* Father = pop(openNodes);
			insertCFGBlock(start, breakBlock);
		return breakBlock;
	}
	
	else {
	
		end=ConstructCFGBaseStatement(openNodes,start);


	}
	return end;
}
#pragma endregion

#pragma region  writing the cfgs to the .dot

char* writeDotGraphOperationsTree(controlFlowGraphBlock* cfg, FILE* file) {

	char* treeText = (char*)malloc(sizeof(char));//OT: [ ]\n
	treeText[0] = '\0';
	if (cfg->instructions)
	{
		for (int i = 0; i < (cfg->instructions->size); i++) {

			cfgBlockContent* instruction = cfg->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_OTNODE) {
				//
				char* newText = mystrcat(treeText, "OT: [");
				treeText = (char*)realloc(treeText, (stringLen(newText)) * sizeof(char));
				treeText = newText;

				char* newText20 = printTree(treeText, instruction->ot);
				treeText = newText20;
				char* newText2 = mystrcat(treeText, "]\n");
				treeText = (char*)realloc(treeText, stringLen(newText2) * sizeof(char));
				treeText = newText2;
			}
		}
	}
	return treeText;
}

void writeDotGraphIfStatement(controlFlowGraphBlock* node, FILE* file,controlFlowGraphBlock* start) {
	fprintf(file, "    n%p ", node);
	printBlockToFile(BlockType_STRING[node->blocktype], file, node);
	fprintf(file, "    n%p -> n%p\n", start, node);

	// Determine if there are more than one out nodes
	int isElseNode = node->outNodeCount > 1 ? 1 : 0;


	controlFlowGraphBlock* ThenNode = node->nodes[0];
	fprintf(file, "    n%p -> n%p [label=\"True\" color=\"green\"]\n", node, ThenNode);
		 writeDotGraph(ThenNode, file, node);
	if (isElseNode == 1) {
		controlFlowGraphBlock* ElseNode = node->nodes[1];
		fprintf(file, "    n%p -> n%p [label=\"True\" color=\"red\"]\n", node, ElseNode);

		 writeDotGraph(ElseNode, file,node);


	}
	node->drawn = 0;


	return ;
}
void writeDotGraphWhileStatement(controlFlowGraphBlock* node, FILE* file, controlFlowGraphBlock* start) {

	fprintf(file, "    n%p ", node);
	printBlockToFile(BlockType_STRING[node->blocktype], file, node);
	fprintf(file, "    n%p -> n%p\n", start, node);
	



	controlFlowGraphBlock* whilebody = node->nodes[0];

	fprintf(file, "	   n%p -> n%p [ label = \"True\" color=\"green\"]\n", node, whilebody);
	writeDotGraph(whilebody, file, node);
	controlFlowGraphBlock* exitWhilebody = node->nodes[1];
	fprintf(file, "	   n%p -> n%p [ label = \"False\" color=\"red\"]\n", node, exitWhilebody);
	writeDotGraph(exitWhilebody, file, node);
	node->drawn = 0;

	
	return exitWhilebody;
	

}
void writeDotGraphBaseStatement(controlFlowGraphBlock* node, FILE* file, controlFlowGraphBlock* start) {
		fprintf(file, "    n%p ", node);
		printBlockToFile(BlockType_STRING[node->blocktype], file, node);
		if (start != NULL&& !( node->blocktype==ElseBlock|| node->blocktype == ThenBlock ||node->blocktype == WhileBodyBlock || node->blocktype == WhileExitBlock)) {
			fprintf(file, "    n%p -> n%p\n", start,node );

		}
	

		 writeDotGraph(node->nodes[0], file, node);
		 return ;

}
void printBlockToFile(char* blockType, FILE* file, controlFlowGraphBlock* node) {
	fprintf(file, "[label=\"%s\\n ", blockType);
	if (node->instructions->size > 0) {
		char* treeText = writeDotGraphOperationsTree(node, file);
		fprintf(file, "%s\"]\n", treeText);
	}
	else {
		fprintf(file, "%s\"]\n", "");

	}
}
void writeDotGraph(controlFlowGraphBlock* cfg, FILE* file, controlFlowGraphBlock* start) {


	switch (cfg->blocktype)
	{
	case IfBlock:
		writeDotGraphIfStatement( cfg, file,start);
		break;
	case WhileBlock:
		if (cfg->drawn == 0) {
			cfg->drawn = 1;
			 writeDotGraphWhileStatement(cfg, file, start);
		}
		else if (cfg->drawn==1) {
			cfg->drawn = 2;
			fprintf(file, "    n%p -> n%p\n", start, cfg);

			return ;
		}
		else {
			return;
		}
		break;
	case BreakBlock:
		fprintf(file, "    n%p ", cfg);
		printBlockToFile(BlockType_STRING[cfg->blocktype], file, cfg);

		fprintf(file, "    n%p -> n%p\n", start, cfg);
		if (cfg->outNodeCount > 0) {

		fprintf(file, "    n%p -> n%p\n", cfg, cfg->nodes[0]);
		}
		return ;
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {
			fprintf(file, "    n%p ", cfg);
			printBlockToFile(BlockType_STRING[cfg->blocktype], file, cfg);
				fprintf(file, "    n%p -> n%p\n", start, cfg);
			return ;
		}
		//TODO:
		writeDotGraphBaseStatement( cfg, file,start);

		break;
	default:
		if (cfg->blocktype == IfExitBlock) {
			cfg->drawn++;
		}
		if (cfg->blocktype == IfExitBlock&&cfg->drawn>1) {
			fprintf(file, "    n%p -> n%p\n", start, cfg);

			break;
		}
	
		if (cfg->outNodeCount <= 0) {
			fprintf(file, "    n%p ", cfg);
			printBlockToFile(BlockType_STRING[cfg->blocktype], file, cfg);
			fprintf(file, "    n%p -> n%p\n", start, cfg);
			return ;
		}
		writeDotGraphBaseStatement( cfg, file,start);
		break;
	}
	return;


}


void CFGToDotFile(controlFlowGraphBlock* cfg, char* fileName) {
	FILE* file = fopen(fileName, "w");
	if (!file) {
		perror("Error opening file");
		return;
	}
	fprintf(file, "digraph G {\n");
	const char* graphSettings = "ordering=out;\n"
		"ranksep=.4;\n"
		"bgcolor=\"lightgrey\";  node [shape=box, fixedsize=false, fontsize=12, fontname=\"Helvetica-bold\", fontcolor=\"blue\"\n"
		"width=.25, height=.25, color=\"black\", fillcolor=\"white\", style=\"filled, solid, bold\"];\n"
		"\n"
		"edge [arrowsize=.5, color=\"black\", style=\"bold\"]";
	fprintf(file, "% s", graphSettings);
	writeDotGraph(cfg, file,NULL);
	// Write each control flow graph to the dot file
	fprintf(file, "}\n");



	fclose(file);
}

#pragma endregion

#pragma region helper functions
// some helper functions with strings
char* mystrcat(const char* str1, const char* str2)
{


	char* dest = NULL;

	if (str1 && str2)
	{
		int str1_length = stringLen(str1);
		int str2_length = stringLen(str2);

		dest = realloc(dest, (str1_length + str2_length + 1) * sizeof(char));
		if (dest)
		{
			memcpy(dest, str1, str1_length);
			memcpy(dest + str1_length, str2, str2_length);
		}
		dest[str1_length + str2_length] = '\0';
	}
	if (dest != NULL) {
		return dest;
	}
}
int stringLen(char* str)
{
	int length = 0;

	// Loop till the NULL character is found
	while (*str != '\0')
	{
		length++;

		// Move to the next character
		str++;
	}
	return length;
}
#include <stdio.h>

void intToStr(int num, char* str) {
	int i = 0;
	int isNegative = 0;

	// Handle negative numbers
	if (num < 0) {
		isNegative = 1;
		num = -num;
	}

	// Convert integer to string
	do {
		str[i++] = (num % 10) + '0';
		num /= 10;
	} while (num > 0);

	if (isNegative) {
		str[i++] = '-';
	}

	str[i] = '\0';  // Null-terminate the string

	// Reverse the string
	for (int j = 0; j < i / 2; j++) {
		char temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
}
char* remove_last_three_chars(const char* fileName) {
	size_t len = strlen(fileName);

	// Check if the file name has at least 3 characters
	if (len <= 4) {
		return NULL; // or return an empty string, depending on your needs
	}

	// Allocate memory for the new string
	char* newFileName = (char*)malloc(len - 4 + 1); // +1 for the null terminator
	if (newFileName == NULL) {
		return NULL; // Memory allocation failed
	}

	// Copy the string excluding the last three characters
	strncpy(newFileName, fileName, len - 4);
	newFileName[len - 4] = '\0'; // null-terminate the new string

	return newFileName;
}

#pragma endregion


