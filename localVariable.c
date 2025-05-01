#include "localVariable.h"


int varCounter = 0;

// Function to create a new node
VarNode* createNode(char* id, Type* type , char * fileName) {
    VarNode* newNode = (VarNode*)malloc(sizeof(VarNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed");
            exit(1);
    }
    //newNode->data = data;
	newNode->id = id;
	newNode->type = type;
	newNode->offset = varCounter;
	varCounter++;
	
    newNode->next = NULL;
	newNode->fileName = fileName;
    return newNode;
}

// Function to add a node to the end of the linked list
void addNode(VarNode** head, char* id, Type* type , char* fileName) {
    VarNode* newNode = createNode(id, type  , fileName);
    if (*head == NULL) {
        *head = newNode;
		newNode->cntVars = 0;
    }
    else {
        VarNode* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to free the linked list
void freeList(VarNode* head) {
    VarNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;

        // Free the varDeclaration
        free(temp->id); //
        free(temp->type); 
        free(temp);
    }
}
FunctionVariables** getLocalVars(Subroutine** subroutines, int cnt, char * fileName) {

	FunctionVariables ** funcVars= malloc(sizeof(FunctionVariables*) * cnt);// a list of all the subprograms within one file
	

	for (int i = 0; i < cnt; i++) {
		VarNode* localVars = NULL;
		funcVars[i] = malloc(sizeof(FunctionVariables));
		funcVars[i]->funcName = subroutines[i]->name;

		
		VarNode* paramerers = NULL;
		varCounter = 1;
		for (int j= 0; j < subroutines[i]->signatureDetails->cntArgs;j++) {


			addNode(&paramerers, subroutines[i]->signatureDetails->arguments[j]->name, subroutines[i]->signatureDetails->arguments[j]->type, fileName);
		}
		
		funcVars[i]->parameters = paramerers;
		varCounter = 1;
		localVars =traverseCfg( subroutines[i]->cfg, NULL, localVars, fileName) ;
		
		// local variables of a certain func are added to it 
		funcVars[i]->localVariables = localVars;

		funcVars[i]->cntVars = varCounter-1;
		funcVars[i]->cntArgs = subroutines[i]->signatureDetails->cntArgs;
    }

	return funcVars;
}
VarNode* newVar(controlFlowGraphBlock* node, VarNode * var ,char* fileName) {
	if (node->instructions)
	{
		for (int i = 0; i < (node->instructions->size); i++) {

			cfgBlockContent* instruction = node->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_VARDECLARATION) {
				for (int j = 0; j < instruction->varDec->cntId; j++) {
			
				addNode(&var, instruction->varDec->Ids[j], instruction->varDec->type  , fileName);
				var->cntVars++;
				}
			}
		}
	}
	return var;
}

VarNode* traverseCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, VarNode * var , char* fileName) {

	var=newVar(node, var  , fileName);
	controlFlowGraphBlock* ThenNode = node->nodes[0];
	var=traverseCfg(ThenNode, node, var , fileName);
	int isElseNode = node->outNodeCount > 1 ? 1 : 0;

	if (isElseNode == 1) {
		controlFlowGraphBlock* ElseNode = node->nodes[1];
		var=traverseCfg(ElseNode, node, var  , fileName);
	}
	node->drawn = 0;
	return var;
}
VarNode* traverseCfgWhileStatement(controlFlowGraphBlock* node,controlFlowGraphBlock* start, VarNode * var , char* fileName) {
	var=newVar(node, var  , fileName);

	controlFlowGraphBlock* whilebody = node->nodes[0];
	var=traverseCfg(whilebody, node, var  , fileName);
	controlFlowGraphBlock* exitWhilebody = node->nodes[1];
	var=traverseCfg(exitWhilebody, node, var  ,  fileName);
	node->drawn = 0;
	return var;
}
VarNode* traverseCfgBaseStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, VarNode * var , char* fileName) {
	var=newVar(node, var , fileName);

	var=traverseCfg(node->nodes[0], node, var  , fileName);
	return var;

}

VarNode* traverseCfg(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start, VarNode* var ,char* fileName) {
	switch (cfg->blocktype)
	{
	case IfBlock:
		var=traverseCfgIfStatement(cfg, start, var ,fileName);
		break;
	case WhileBlock:
		if (cfg->drawn == 0) {
			cfg->drawn = 1;
			var=traverseCfgWhileStatement(cfg, start, var , fileName);
		}
		else if (cfg->drawn == 1) {
			cfg->drawn = 2;
			return var;
		}
		else {
			return var;
		}
		break;
	case BreakBlock:
		var=newVar(cfg, var  ,  fileName);

		return var;
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {
			var=newVar(cfg, var  ,  fileName);

			return var;
		}
		var=traverseCfgBaseStatement(cfg, start, var  ,  fileName);
		break;
	default:
		if (cfg->blocktype == IfExitBlock) {
			cfg->drawn++;
		}
		if (cfg->blocktype == IfExitBlock && cfg->drawn > 1) {
			cfg->drawn = 0;
			break;
		}

		if (cfg->outNodeCount <= 0) {
			var=newVar(cfg, var , fileName);

			return var;
		}
		var=traverseCfgBaseStatement(cfg, start,var   ,  fileName);
		break;
	}
	return var;

}




