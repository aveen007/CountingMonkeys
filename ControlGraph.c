#include "ControlGraph.h"
#include "parser.h"
#include <stdlib.h>


# pragma region subprogram detail construction


Subroutine DefineSubprogram(char* fileName, ParseTree* tree) {


	if (tree) {
		// get the funcDefs, attach each one to a subroutine and get for each subroutine the required details
		  // construct the proper cfg for each subroutine
		tree = tree->children[0];
		Subroutine** subprograms = (Subroutine**)malloc(sizeof(Subroutine*) * tree->childrenCount);
		for (int i = 0; i < tree->childrenCount; i++) {
			subprograms[i] = (Subroutine*)malloc(sizeof(Subroutine));
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
							char* temp = argsTree->children[j]->children[0]->token;
							subprograms[i]->signatureDetails->arguments[j] = (ArgumentDef*)malloc(sizeof(ArgumentDef));
							if (temp) {
								subprograms[i]->signatureDetails->arguments[i]->name = name;
								//setName(args[j]->name, name);
							}

							if (argsTree->childrenCount > 1) {

								subprograms[i]->signatureDetails->arguments[j]->type = HandleType(argsTree->children[1]);
							}



							//TODO : break blocks 
							//TODO: make this work for multiple files instead of 1
							//TODO: capture the errors
								//the error types are ->
										//1. break into nothing
										//2. assign into a literal, (literal is an lvalue in code)
										//3. use a function that is not initialized (not in the cfg already)
								// return the errors as well 
							///Questions
							// TODO:  debug the strange errors that keep coming up

						}
					}
				}
				subprograms[i]->signatureDetails->position = pos;
				subprograms[i]->name = name;


			}



		}
	}

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

	Type* returnType = (Type*)malloc(sizeof(Type));
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

// Function to print the tree in a readable format (in-order traversal)
char* printTree(char* treeText, OTNode* node) {
	if (node == NULL) {
		return treeText;
	}

	if (node->type == NODE_TYPE_OPERATOR) {


		strcat(treeText, " ");
		strcat(treeText, node->value.operator);
		strcat(treeText, "\0");
		strcat(treeText, "(");

		// Print the left child
		treeText = printTree(treeText, node->left);

		// Add operator
		strcat(treeText, " , ");

		// Print the right child
		treeText = printTree(treeText, node->right);

		strcat(treeText, ")");
	}
	else {
		// It's an operand
		strcat(treeText, (char*)node->value.operand);
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
	OTNode* OT = (OTNode*)malloc(sizeof(OTNode));
	if (base->childrenCount > 0) {
		OT = createOperatorNode(base->token);
		OT->left = HandleOperationsTree(base->children[1]);
		OT->right = HandleOperationsTree(base->children[0]);
	}
	else {
		OT = createOperandNode(base->token);
	}
	return OT;

}
void InsertInstruction(Instructions* instructions, cfgBlockContent NewContent) {
	instructions->size++;
	instructions->content = (cfgBlockContent**)realloc(instructions->content, sizeof(cfgBlockContent*) * instructions->size);
	instructions->content[instructions->size - 1] = malloc(sizeof(cfgBlockContent)); // Allocate memory for the new content
	*(instructions->content[instructions->size - 1]) = NewContent; // Copy the new content into the allocated space
}
Instructions* CreateInstructions() {

	Instructions* instructions = malloc(sizeof(Instructions));
	instructions->content = malloc(instructions->content, sizeof(cfgBlockContent*));
	instructions->size = 0;
	return instructions;
}



void insertCFGBlock(controlFlowGraphBlock* nodes, controlFlowGraphBlock* node) {
	if (nodes->outNodeCount < 0) {
		nodes->nodes = createCFGBlock(nodes);
	}

	nodes->outNodeCount++;
	nodes->nodes = (controlFlowGraphBlock**)realloc(nodes->nodes, (sizeof(controlFlowGraphBlock*) * nodes->outNodeCount));
	/*   if (nodes->nodes == NULL) {
		   return;
	   }*/
	nodes->nodes[nodes->outNodeCount - 1] = malloc(sizeof(controlFlowGraphBlock));
	nodes->nodes[nodes->outNodeCount - 1] = node;

}
controlFlowGraphBlock** createCFGBlock(controlFlowGraphBlock* block) {
	//= malloc(sizeof(controlFlowGraphBlock));

	block->nodes = (controlFlowGraphBlock**)malloc(sizeof(controlFlowGraphBlock*));
	//instructions->content = malloc(instructions->content, sizeof(cfgBlockContent*));
	block->outNodeCount = 0;
	return block->nodes;
}

#pragma endregion

#pragma region Queue


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

void CFGInterfacer(char* fileName, ParseTree* tree) {

	if (tree) {
		tree = tree->children[0];
		controlFlowGraphBlock** cfgs = malloc(sizeof(controlFlowGraphBlock*) * tree->childrenCount);


		for (int i = 0; i < tree->childrenCount; i++) {
			// here I am just creating the start block
			controlFlowGraphBlock* cfg = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
			cfg->blocktype = BaseBlock;
			cfg->ast = tree->children[i];
			cfg->instructions = CreateInstructions(cfg->instructions);

			/// ======================================================= run through the childeren

			for (int j = 1; j < cfg->ast->childrenCount; j++) {

				ConstructCFG(cfg, cfg->ast->children[j], BaseBlock);
			}
			/// ==========================================================

			// here I am just creating the end block
			controlFlowGraphBlock* cfgEnd = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
			cfgEnd->blocktype = BaseBlock;
			cfgEnd->ast = tree->children[i];
			cfgEnd->instructions = CreateInstructions(cfg->instructions);
			insertCFGBlock(cfg, cfgEnd);

			cfgs[i] = malloc(sizeof(controlFlowGraphBlock*));
			cfgs[i] = cfg;
			///================================================================================


			char filename[20]; // Make sure this is large enough to hold the filename
			sprintf(filename, "cfgi%d.dot", i); // Create the filename "cfgiX.dot" where X is the index
			CFGToDotFile(cfgs[i], filename);
			// Prepare output picture file name
			char pngFilename[30]; // Adjust size according to your needs
			sprintf(pngFilename, "../cpoCompilerWin/%s.png", filename); // Assuming fileName variable is filename
			char makeTreeGraph[100]; // Increase size for the command string
			sprintf(makeTreeGraph, "dot -Tpng %s -o %s", filename, pngFilename);
			system(makeTreeGraph);


			///=======================================================================================

		}
	}
}

void ConstructCFG(controlFlowGraphBlock* cfg, ParseTree* tree, BlockType  blockType) {



	ParseTree* instructionTree = tree;
	if (strcmp(instructionTree->token, "VarStatement") == 0) {
		// add a var instruction
		char** ids = (char*)malloc(sizeof(char*) * instructionTree->children[0]->childrenCount);
		for (int k = 0; k < instructionTree->children[0]->childrenCount; k++) {

			ids[k] = instructionTree->children[0]->children[k]->token;
		}
		Type* instructionType = HandleType(instructionTree->children[1]);
		varDeclaration* varDecl = CreateVarDeclaration(ids, instructionType);
		cfgBlockContent instructionContent;
		instructionContent.type = TYPE_VARDECLARATION;
		instructionContent.varDec = varDecl;  // Assign var declaration to the union
		InsertInstruction(cfg->instructions, instructionContent);

	}
	else if (strcmp(instructionTree->token, "Expression") == 0) {
		// add an operations tree
		cfgBlockContent instructionContent;

		OTNode* otNode = HandleOperationsTree(instructionTree->children[0]);

		instructionContent.type = TYPE_OTNODE;
		instructionContent.ot = otNode;  // Assign OTNode to the union

		InsertInstruction(cfg->instructions, instructionContent);
	}
	else if (strcmp(instructionTree->token, "IfStatement") == 0)
	{
		//insert the last instructions w the last cfg to the graph

		// initialize a new cfg node

		controlFlowGraphBlock* IfStatementCfg = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
		IfStatementCfg->blocktype = IfBlock;

		//here we just add the condition to the if blocks 

		IfStatementCfg->ast = instructionTree;
		IfStatementCfg->instructions = CreateInstructions();
		cfgBlockContent instructionContent;

		OTNode* otNode = HandleOperationsTree(instructionTree->children[0]->children[0]);
		instructionContent.ot = otNode;
		instructionContent.type = TYPE_OTNODE;
		InsertInstruction(IfStatementCfg->instructions, instructionContent);


		//
		int k = 1;
		IfStatementCfg->nodes = createCFGBlock(IfStatementCfg);
		IfStatementCfg->blocktype = IfBlock;
		IfStatementCfg->ast = instructionTree;
		controlFlowGraphBlock* thenBlock = malloc(sizeof(controlFlowGraphBlock));
		thenBlock->instructions = CreateInstructions();
		thenBlock->blocktype = ThenBlock;
		thenBlock->ast = instructionTree->children[0];
		for (; k < instructionTree->childrenCount - 1; k++) {
			ConstructCFG(thenBlock, instructionTree->children[k], ThenBlock);

		}
		if (strcmp(instructionTree->children[k]->token, "ElseStatement") != 0) {

			ConstructCFG(thenBlock, instructionTree->children[k], ThenBlock);

		}
		else {
			controlFlowGraphBlock* elseBlock = malloc(sizeof(controlFlowGraphBlock));

			elseBlock->instructions = CreateInstructions();
			for (int j = 0; j < instructionTree->children[k]->childrenCount; j++) {
				ConstructCFG(elseBlock, instructionTree->children[k]->children[j], ElseBlock);
			}
			elseBlock->blocktype = ElseBlock;
			elseBlock->ast = instructionTree->children[k];

			insertCFGBlock(IfStatementCfg, elseBlock);
		}
		insertCFGBlock(IfStatementCfg, thenBlock);



		if (cfg->outNodeCount < 0) {
			cfg->nodes = createCFGBlock(cfg);

		}
		insertCFGBlock(cfg, IfStatementCfg);

	}
	else if (strcmp(instructionTree->token, "WhileStatement") == 0) {

		// initialize a new cfg node

		controlFlowGraphBlock* WhileBlockCfg = (controlFlowGraphBlock*)malloc(sizeof(controlFlowGraphBlock));
		WhileBlockCfg->blocktype = WhileBlock;

		//here we just add the condition to the if blocks 

		WhileBlockCfg->ast = instructionTree;
		WhileBlockCfg->instructions = CreateInstructions();
		cfgBlockContent instructionContent;

		OTNode* otNode = HandleOperationsTree(instructionTree->children[0]->children[0]);
		instructionContent.ot = otNode;
		instructionContent.type = TYPE_OTNODE;
		InsertInstruction(WhileBlockCfg->instructions, instructionContent);



		//TODO: 
		controlFlowGraphBlock* baseBlock = malloc(sizeof(controlFlowGraphBlock));

		baseBlock->instructions = CreateInstructions();
		baseBlock->blocktype = WhileBodyBlock;
		baseBlock->ast = instructionTree;
		for (int j = 1; j < instructionTree->childrenCount; j++) {
			ConstructCFG(baseBlock, instructionTree->children[j], WhileBodyBlock);
		}
		WhileBlockCfg->nodes = createCFGBlock(WhileBlockCfg);

		insertCFGBlock(WhileBlockCfg, baseBlock);
		insertCFGBlock(cfg, WhileBlockCfg);
	}


}

#pragma endregion


#pragma region  writing the cfgs to the .dot



controlFlowGraphBlock* writeDotGraph(Stack* openNodes, FILE* file) {
	if (isEmpty(openNodes) || !file) { return; }
	controlFlowGraphBlock* cfg = pop(openNodes);
	// Begin the dot file

	char* treeText = (char*)malloc(sizeof(char));
	treeText[0] = '\0'; // Initialize with an empty string
	//int currentSize = 1; // To hold the size of the dynamic string
	if (cfg->instructions)
	{

		for (int i = 0; i < (cfg->instructions->size); i++) {

			cfgBlockContent* instruction = cfg->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_OTNODE) {
				//
				strcat(treeText, "OT: [");

				treeText = printTree(treeText, instruction->ot);
				strcat(treeText, "]\n");
			}
			// TODO: here I want to create some special code to write the operations tree and the var statements inside
			// the block
		}
	}

	// Print the current block

	fprintf(file, "    n%p [label=\"%s\\n %s\"]\n", cfg,
		(cfg->blocktype == BaseBlock ? "BaseBlock" :
			(cfg->blocktype == IfBlock ? "IfBlock" :
				(cfg->blocktype == WhileBlock ? "WhileBlock" :
					(cfg->blocktype == ElseBlock ? "ElseBlock" :
						(cfg->blocktype == ThenBlock ? "ThenBlock" : (cfg->blocktype == WhileBodyBlock ? "WhileBodyBlock" :
							"BreakBlock")))))),
		treeText);

	///

	if (cfg->blocktype == IfBlock) {
		// Determine if there are more than one out nodes
		int isElseNode = cfg->outNodeCount > 1 ? 1 : 0;

		for (int j = 0; j < cfg->outNodeCount; j++) {
			// Check if this is the "then" or "else" node
			if (isElseNode == 1 && j == 0) {
				// This is the "else" case
				fprintf(file, "    n%p -> n%p [label=\"False\" color=\"red\"]\n", cfg, cfg->nodes[j]);
				controlFlowGraphBlock* ElseNode = cfg->nodes[j];
				controlFlowGraphBlock* FathersNext = peek(openNodes);

				push(openNodes, ElseNode);
				controlFlowGraphBlock* finalOfElse = writeDotGraph(openNodes, file);
				        fprintf(file, "    n%p -> n%p [ color=\"blue\"]\n", finalOfElse, FathersNext);



			}
			else {
				fprintf(file, "    n%p -> n%p [label=\"True\" color=\"green\"]\n", cfg, cfg->nodes[j]);
				controlFlowGraphBlock* ThenNode = cfg->nodes[j];
				controlFlowGraphBlock* FathersNext = peek(openNodes);

				push(openNodes, ThenNode);
				controlFlowGraphBlock* finalOfThen = writeDotGraph(openNodes, file);
				fprintf(file, "    n%p -> n%p [ color=\"blue\"]\n", finalOfThen, FathersNext);
				// This is the "then" case
			}

		}
	}
	else if (cfg->blocktype == WhileBlock) {


		fprintf(file, "    n%p -> n%p [label=\"True\" color=\"black\"]\n", cfg->nodes[0], cfg);
	}
	//Traverse out nodes and create edges

	if (cfg->outNodeCount > 0) {
		if (cfg->blocktype != IfBlock && cfg->blocktype != WhileBodyBlock) {

			fprintf(file, "    n%p -> n%p\n", cfg, cfg->nodes[0]);
			for (int i = cfg->outNodeCount - 1; i >= 0; i--)
			{
				controlFlowGraphBlock* outBlock = cfg->nodes[i]; // Assuming the node holds a pointer to a block

				push(openNodes, outBlock);


			}
		
			while (!isEmpty(openNodes)) {
				//fprintf(file, "    n%p -> n%p [ color=\"yellow\"]\n", cfg, peek(openNodes));

				writeDotGraph(openNodes, file);

			}
		
		}
		

	}
	else {
		return cfg;
	}
	//while (!isEmpty(openNodes)) {
	//  /*  if (cfg->outNodeCount <= 0) {
	//        fprintf(file, "    n%p -> n%p [ color=\"blue\"]\n", cfg, peek(openNodes));
	//    }*/

	//    
	//}


	// End the dot file
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
	Stack* openNodes = createStack();
	push(openNodes, cfg);
	writeDotGraph(openNodes, file);
	// Write each control flow graph to the dot file
	fprintf(file, "}\n");



	fclose(file);
}

#pragma endregion

