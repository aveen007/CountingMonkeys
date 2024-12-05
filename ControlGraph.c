#include "ControlGraph.h"
#include "parser.h"
#include <stdlib.h>
#define allocator_may_return_null 1 




# pragma region subprogram detail construction
# pragma region create subroutine info and errors
ErrorInfoCFG* errors;
CfgsInfo* createCfgsInfo(ParseTree* ast) {
	CfgsInfo* info = malloc(sizeof(CfgsInfo));
	info->cfgs = createCfgs(ast);
	info->errors = NULL;
	return info;
}
controlFlowGraphBlock** createCfgs(ParseTree* ast) {
	controlFlowGraphBlock** cfgs = (controlFlowGraphBlock**)malloc(sizeof(controlFlowGraphBlock*) * ast->childrenCount);
	return cfgs;
}
Subroutine** createSubroutines(ParseTree* ast) {
	Subroutine** subroutines = (Subroutine**)malloc(sizeof(Subroutine*) * ast->childrenCount);
	for (int i = 0; i < ast->childrenCount; i++) {
		subroutines[i] = malloc(sizeof(Subroutine));
	}
	return subroutines;
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
							//char* temp = argsTree->children[j]->children[0]->token;
							subprograms[i]->signatureDetails->arguments[j] = (ArgumentDef*)malloc(sizeof(ArgumentDef));
							//if (temp) {
								subprograms[i]->signatureDetails->arguments[j]->name = argsTree->children[j]->children[0]->token;
								//setName(args[j]->name, name);
							//}

							if (argsTree->childrenCount > 1) {

								subprograms[i]->signatureDetails->arguments[j]->type = HandleType(argsTree->children[1]);
							}



							//TODO:2 capture the errors
								//the error types are ->
										//2. assign into a literal, (literal is an lvalue in code)
										//3. use a function that is not initialized (not in the cfg already)
								// return the errors as well 
							//TODO:3 check the callOfIndex
							//TODO:4 call graph
							//TODO:5 the tree can have more than two operands aka callOfIndex

							///Questions
							//when should we capture the error 
							//the structure of the errors
							// check if I have the right structure for the cfg
							//Payload
							// add an enumeration for the type of the operation in ot
							// rethink the out nodes struct
							//error tree nodes, maybe I can add the errors to evry one of the 
							// corresponding cfgs and enter the list to construct and draw
							//the nodes for the tree can have args** insted of left and right
							//does the structure of the cfg affect the 3rd lab because I have a different struct 

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

#pragma region  handling types for the subprograms


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
#pragma endregion

# pragma region operations tree



// Function to create a new node for an operator
OTNode* createOperatorNode(char* operator) {
	OTNode* newNode = (OTNode*)malloc(sizeof(OTNode));
	newNode->type = NODE_TYPE_OPERATOR;
	newNode->value.operator = operator;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

// Function to create a new node for an operand
OTNode* createOperandNode(char* operand) {
	OTNode* newNode = (OTNode*)malloc(sizeof(OTNode));
	newNode->type = NODE_TYPE_OPERAND;
	newNode->value.operand = operand;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

// Function to free the operations tree
void freeTree(OTNode* root) {
	if (root) {
		freeTree(root->left);
		freeTree(root->right);
		free(root);
	}
}
size_t estimatedSize(OTNode* node) {
	if (node == NULL) {
		return 1; // For empty string
	}
	size_t size = 0;
	if (node->type == NODE_TYPE_OPERATOR) {
		size += strlen(node->value.operator) + 3; // operator + " , " + parentheses
		size += estimatedSize(node->left);
		size += estimatedSize(node->right);
	}
	else {
		size += strlen(node->value.operand);
	}
	return size;
}

// Function to print the tree in a readable format (in-order traversal)
char* printTree(char* treeText, OTNode* node) {
	//int size = estimatedSize(node) + 1; // +1 for null-terminator

	 //treeText = realloc(treeText, sizeof (char*)*(strlen(treeText)+size));
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
		if (node->left != NULL && node->right != NULL) {
			char* newText4 = mystrcat(treeText, "(");
			treeText = (char*)realloc(treeText, stringLen(newText4) * sizeof(char));
			treeText = newText4;

			char* newText40 = mystrcat(treeText, "\0");
			treeText = (char*)realloc(treeText, stringLen(newText40) * sizeof(char));
			treeText = newText40;

			// Print the left child
			treeText = printTree(treeText, node->left);

			// Add operator
			char* newText5 = mystrcat(treeText, " , ");
			treeText = (char*)realloc(treeText, stringLen(newText5) * sizeof(char));
			treeText = newText5;

			//strcat(treeText, " , ");

			// Print the right child
			treeText = printTree(treeText, node->right);
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

varDeclaration* CreateVarDeclaration(char** Ids, Type* type) {
	varDeclaration* var = (varDeclaration*)malloc(sizeof(varDeclaration));
	var->Ids = Ids;
	var->type = type;

	return var;

}
OTNode* HandleOperationsTree(ParseTree* base) {
	OTNode* OT = createOperatorNode(base->token);
	if (base->childrenCount > 0) {
		OT->left = HandleOperationsTree(base->children[1]);
		OT->right = HandleOperationsTree(base->children[0]);
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
	/*   if (nodes->nodes == NULL) {
		   return;
	   }*/
	   //nodes->nodes[nodes->outNodeCount - 1] = malloc(sizeof(controlFlowGraphBlock));
	nodes->nodes[nodes->outNodeCount - 1] = node;

}
controlFlowGraphBlock* createCFGBlock(ParseTree* ast, BlockType blockType) {
	//= malloc(sizeof(controlFlowGraphBlock));

	controlFlowGraphBlock* block = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
	block->nodes = (controlFlowGraphBlock**)malloc(sizeof(controlFlowGraphBlock*));
	//instructions->content = malloc(instructions->content, sizeof(cfgBlockContent*));
	block->outNodeCount = 0;
	block->ast = ast;
	block->instructions = CreateInstructions();
	block->blocktype = blockType;

	return block;
}

#pragma endregion

#pragma region Stack


// Function to create a stack
Stack* createStack() {
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	stack->capacity = 2; // Initial capacity
	stack->top = -1;
	stack->items = (controlFlowGraphBlock**)malloc(stack->capacity * sizeof(controlFlowGraphBlock*));
	return stack;
}

// Function to check if the stack is empty
int isEmpty(Stack* stack) {
	return stack->top == -1;
}

// Function to double the capacity of the stack
void resizeStack(Stack* stack) {
	stack->capacity *= 2;
	stack->items = (controlFlowGraphBlock**)realloc(stack->items, stack->capacity * sizeof(controlFlowGraphBlock*));
}

// Function to push an item onto the stack
void push(Stack* stack, controlFlowGraphBlock* block) {
	if (stack->top + 1 >= stack->capacity) {
		resizeStack(stack);
	}
	stack->items[++stack->top] = block;
}

// Function to pop an item from the stack
controlFlowGraphBlock* pop(Stack* stack) {
	if (isEmpty(stack)) {
		return NULL; // Stack is empty
	}
	return stack->items[stack->top--];
}

// Function to peek at the top item of the stack without removing it
controlFlowGraphBlock* peek(Stack* stack) {
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

CfgsInfo* CFGInterfacer(char* fileName, ParseTree* tree) {

	if (tree) {
		tree = tree->children[0];
		CfgsInfo* cfgsInfo = createCfgsInfo(tree);
		controlFlowGraphBlock** cfgs = cfgsInfo->cfgs;

		for (int i = 0; i < tree->childrenCount; i++) {
			// here I am just creating the start block
			controlFlowGraphBlock* cfg = createCFGBlock(tree->children[i], BaseBlock);
			ConstructCFG(cfg, tree->children[i],cfgsInfo->errors);
			controlFlowGraphBlock* cfgEnd = createCFGBlock(tree->children[i], BaseBlock);
			insertCFGBlock(cfg, cfgEnd);
			cfgs[i] = cfg;
			//========================================
			char* filename;
			size_t filename_size = (size_t)snprintf(NULL, 0, "cfgi%d.dot", i);
			filename = malloc(filename_size + 1);
			snprintf(filename, filename_size + 1, "cfgi%d.dot", i);
			CFGToDotFile(cfgs[i], filename,cfgsInfo->errors);
			// Prepare output picture file name
			size_t pngFileSize = (size_t)snprintf(NULL, 0, "../cpoCompilerWin/%s.png", filename);
			char* pngFilename = malloc(pngFileSize + 1); // Adjust size according to your needs
			snprintf(pngFilename, pngFileSize + 1, "../cpoCompilerWin/%s.png", filename);
			size_t makeTreeSize = (size_t)snprintf(NULL, 0, "dot -Tpng %s -o %s", filename, pngFilename);
			char* makeTreeGraph = malloc(makeTreeSize + 1); // Increase size for the command string
			snprintf(makeTreeGraph, makeTreeSize + 1, "dot -Tpng %s -o %s", filename, pngFilename);
			system(makeTreeGraph);
			///=======================================================================================

		}
		cfgsInfo->errors = errors;
		return cfgsInfo;
	}
	return;
}
cfgBlockContent* createInstructionsVarStatement(ParseTree* ast ) {
	// add a var instruction
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));
	char** ids = (char*)malloc(sizeof(char*) * ast->children[0]->childrenCount);
	for (int k = 0; k < ast->children[0]->childrenCount; k++) {

		ids[k] = ast->children[0]->children[k]->token;
	}
	Type* instructionType = HandleType(ast->children[1]);
	varDeclaration* varDecl = CreateVarDeclaration(ids, instructionType);
	content->type = TYPE_VARDECLARATION;
	content->varDec = varDecl;  // Assign var declaration to the union
	return content;
}
cfgBlockContent* createInstructionsExpression(ParseTree* ast ) {
	// add an operations tree
	cfgBlockContent* content = malloc(sizeof(cfgBlockContent));

	OTNode* otNode = HandleOperationsTree(ast->children[0]);

	content->type = TYPE_OTNODE;
	content->ot = otNode;  // Assign OTNode to the union
	return content;

}
int ImWhile = 0;
controlFlowGraphBlock* ConstructCFGIfStatement(ParseTree* tree ) {
	controlFlowGraphBlock* IfStatementCfg = createCFGBlock(tree, IfBlock);
	InsertInstruction(IfStatementCfg->instructions, tree->children[0]);

	controlFlowGraphBlock* thenBlock = createCFGBlock(tree->children[1], ThenBlock);

	for (int j = 0; j < tree->children[1]->childrenCount; j++) {

		ConstructCFG(thenBlock, tree->children[1]->children[j] );
	}
	insertCFGBlock(IfStatementCfg, thenBlock);

	if (tree->childrenCount > 2) {
		controlFlowGraphBlock* elseBlock = createCFGBlock(tree->children[2], ElseBlock);
		for (int j = 0; j < tree->children[2]->childrenCount; j++) {
			ConstructCFG(elseBlock, tree->children[2]->children[j] );
		}
		insertCFGBlock(IfStatementCfg, elseBlock);
		controlFlowGraphBlock* ifExitBlock = createCFGBlock(tree->children[2], IfExitBlock);
		insertCFGBlock(IfStatementCfg, ifExitBlock);
	}
	else {
		controlFlowGraphBlock* ifExitBlock = createCFGBlock(tree->children[1], IfExitBlock);
		insertCFGBlock(IfStatementCfg, ifExitBlock);
	}
	return IfStatementCfg;

}
controlFlowGraphBlock* ConstructCFGWhileStatement(ParseTree* tree ) {
	// initialize a new cfg node

	controlFlowGraphBlock* WhileBlockCfg = createCFGBlock(tree, WhileBlock);
	InsertInstruction(WhileBlockCfg->instructions, tree->children[0]);
	controlFlowGraphBlock* whileBodyBlock = createCFGBlock(tree, WhileBodyBlock);

	for (int j = 1; j < tree->childrenCount; j++) {
		ConstructCFG(whileBodyBlock, tree->children[j], WhileBodyBlock );
	}
	controlFlowGraphBlock* exitBlock = createCFGBlock(tree, WhileExitBlock);
	insertCFGBlock(WhileBlockCfg, whileBodyBlock);
	insertCFGBlock(WhileBlockCfg, exitBlock);
	ImWhile = 0;
	return WhileBlockCfg;
}
void ConstructCFG(controlFlowGraphBlock* cfg, ParseTree* tree ) {

	controlFlowGraphBlock* next;
	if (strcmp(tree->token, "IfStatement") == 0)
	{
		next = ConstructCFGIfStatement(tree );
		insertCFGBlock(cfg, next);
	}
	else if (strcmp(tree->token, "WhileStatement") == 0) {
		ImWhile = 1;
		next = ConstructCFGWhileStatement(tree );
		insertCFGBlock(cfg, next);
	}
	else if (strcmp(tree->token, "BreakStatement") == 0) {
		if (ImWhile == 0) {
			ErrorInfoCFG* errorBreak = createErrorInfoNodeCFG("Break not in a loop", tree->line, tree->position);
			addErrorCFG(&errors, errorBreak);
		}
		next = createCFGBlock(tree, BreakBlock);;
		insertCFGBlock(cfg, next);
	}
	else {
		InsertInstruction(cfg->instructions, tree);
		for (int i = 0; i < tree->childrenCount; i++)
		{
			if (strcmp(tree->children[i]->token, "FuncSignature") == 0)
				continue;
			ConstructCFG(cfg, tree->children[i] );
		}
	}
}

#pragma endregion



#pragma region  writing the cfgs to the .dot

char* writeDotGraphOperationsTree(controlFlowGraphBlock* cfg, FILE* file ) {

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
controlFlowGraphBlock* writeDotGraphIfStatement(Stack* openNodes, controlFlowGraphBlock* node, FILE* file ) {

	node = pop(openNodes);
	// Determine if there are more than one out nodes
	int isElseNode = node->outNodeCount > 2 ? 1 : 0;
	// connect to father node
	controlFlowGraphBlock* Father = pop(openNodes);
	fprintf(file, "    n%p -> n%p\n", Father, node);


	controlFlowGraphBlock* IfExitNode;
	controlFlowGraphBlock* ThenNode = node->nodes[0];
	fprintf(file, "    n%p -> n%p [label=\"True\" color=\"green\"]\n", node, ThenNode);
	push(openNodes, ThenNode);
	controlFlowGraphBlock* finalOfThen = writeDotGraph(openNodes, file );
	if (isElseNode == 1) {
		// This is the "else" case
		controlFlowGraphBlock* ElseNode = node->nodes[1];
		fprintf(file, "    n%p -> n%p [label=\"False\" color=\"red\"]\n", node, ElseNode);
		push(openNodes, ElseNode);
		controlFlowGraphBlock* finalOfElse = writeDotGraph(openNodes, file );
		IfExitNode = node->nodes[2];
		fprintf(file, "    n%p -> n%p [color=\"blue\"]\n", finalOfElse, IfExitNode);
	}
	else {

		IfExitNode = node->nodes[1];
	}
	fprintf(file, "    n%p -> n%p [ color=\"blue\"]\n", finalOfThen, IfExitNode);


	fprintf(file, "    n%p ", IfExitNode);
	printBlockToFile(BlockType_STRING[IfExitBlock], file, IfExitNode );
	push(openNodes, IfExitNode);
	return IfExitNode;
}
controlFlowGraphBlock* writeDotGraphWhileStatement(Stack* openNodes, controlFlowGraphBlock* node, FILE* file ) {

	node = pop(openNodes);
	controlFlowGraphBlock* Father = pop(openNodes);
	fprintf(file, "    n%p -> n%p\n", Father, node);

	controlFlowGraphBlock* whilebody = node->nodes[0];
	fprintf(file, "	   n%p -> n%p [ label = \"True\" color=\"green\"]\n", node, whilebody);

	controlFlowGraphBlock* exitWhilebody = node->nodes[1];
	push(openNodes, whilebody);
	controlFlowGraphBlock* FinalOfWhilebody = writeDotGraph(openNodes, file );
	fprintf(file, "    n%p -> n%p [ color=\"blue\"]\n", FinalOfWhilebody, node);
	fprintf(file, "	   n%p -> n%p [ label = \"False\" color=\"red\"]\n", node, exitWhilebody);


	fprintf(file, "    n%p ", exitWhilebody);
	printBlockToFile(BlockType_STRING[WhileExitBlock], file, exitWhilebody );
	push(openNodes, exitWhilebody);
	return exitWhilebody;

}
controlFlowGraphBlock* writeDotGraphBaseStatement(Stack* openNodes, controlFlowGraphBlock* node, FILE* file ) {
	if (node->outNodeCount <= 0) {
		fprintf(file, "    n%p ", node);
		printBlockToFile(BlockType_STRING[node->blocktype], file, node );
		return node;
	}
	controlFlowGraphBlock* outBlock = NULL;
	for (int i = 0; i < node->outNodeCount; i++)
	{
		outBlock = node->nodes[i]; // Assuming the node holds a pointer to a block
		push(openNodes, outBlock);
		outBlock = writeDotGraph(openNodes, file );
		if (outBlock ->blocktype==BreakBlock) {
			return outBlock;
		}

	}
	return outBlock;


}
void printBlockToFile(char* blockType, FILE* file, controlFlowGraphBlock* node ) {
	fprintf(file, "[label=\"%s\\n ", blockType);
	if (node->instructions->size > 0) {
		char* treeText = writeDotGraphOperationsTree(node, file );
		fprintf(file, "%s\"]\n", treeText);
	}
	else {
		fprintf(file, "%s\"]\n", "");

	}
}
controlFlowGraphBlock* writeDotGraph(Stack* openNodes, FILE* file ) {

	// Print the current block
	if (isEmpty(openNodes) || !file) { return; }
	controlFlowGraphBlock* cfg = peek(openNodes);
	fprintf(file, "    n%p ", cfg);
	printBlockToFile(BlockType_STRING[cfg->blocktype], file, cfg );

	switch (cfg->blocktype)
	{
	case IfBlock:
		writeDotGraphIfStatement(openNodes, cfg, file );
		break;
	case WhileBlock:
		writeDotGraphWhileStatement(openNodes, cfg, file );
		break;
	case BreakBlock:
		pop(openNodes);
		if (!isEmpty(openNodes)) {
			controlFlowGraphBlock* Father = pop(openNodes);
			fprintf(file, "    n%p -> n%p\n", Father, cfg);
		}
		return cfg;
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {
			pop(openNodes);
			if (!isEmpty(openNodes)) {
				controlFlowGraphBlock* Father = pop(openNodes);
				fprintf(file, "    n%p -> n%p\n", Father, cfg);
			}
				return cfg;
		}
		writeDotGraphBaseStatement(openNodes, cfg, file );

		break;
	default:
		if (cfg->outNodeCount <= 0) {
			pop(openNodes);
			return cfg;
		}
		writeDotGraphBaseStatement(openNodes, cfg, file );
		break;
	}
}


void CFGToDotFile(controlFlowGraphBlock* cfg, char* fileName ) {
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
	Stack* openNodes = createStack();
	push(openNodes, cfg);
	writeDotGraph(openNodes, file );
	// Write each control flow graph to the dot file
	fprintf(file, "}\n");



	fclose(file);
}
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
#pragma endregion

