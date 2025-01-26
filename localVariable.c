#include "localVariable.h"
// Function to create a new node
Node* createNode(varDeclaration * data, char * fileName) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed");
            exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
	newNode->fileName = fileName;
    return newNode;
}

// Function to add a node to the end of the linked list
void addNode(Node** head, varDeclaration * data, char * fileName) {
    Node* newNode = createNode(data, fileName);
    if (*head == NULL) {
        *head = newNode;
    }
    else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to free the linked list
void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;

        // Free the varDeclaration
        free(temp->data->Ids); // Assuming Ids is dynamically allocated
        free(temp->data->type); // Assuming type is dynamically allocated
        free(temp);
    }
}
Node* getLocalVars(Subroutine** subroutines, int cnt, Node* localVars, char * fileName) {
    for (int i = 0; i < cnt; i++) {
		localVars =traverseCfg( subroutines[i]->cfg, NULL, localVars, fileName) ;
    }
	return localVars;
}
Node* newVar(controlFlowGraphBlock* node, Node * var, char* fileName) {
	if (node->instructions)
	{
		for (int i = 0; i < (node->instructions->size); i++) {

			cfgBlockContent* instruction = node->instructions->content[i]; // Assuming the node holds a pointer to a block
			if (instruction->type == TYPE_VARDECLARATION) {
				addNode(&var, instruction->varDec, fileName);
			}
		}
	}
	return var;
	//TODO: here I will handle adding the var to the var list
}
Node* traverseCfgIfStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, Node * var, char* fileName) {

	var=newVar(node, var, fileName);
	controlFlowGraphBlock* ThenNode = node->nodes[0];
	var=traverseCfg(ThenNode, node, var, fileName);
	int isElseNode = node->outNodeCount > 1 ? 1 : 0;

	if (isElseNode == 1) {
		controlFlowGraphBlock* ElseNode = node->nodes[1];
		var=traverseCfg(ElseNode, node, var, fileName);
	}
	node->drawn = 0;
	return var;
}
Node* traverseCfgWhileStatement(controlFlowGraphBlock* node,controlFlowGraphBlock* start, Node * var, char* fileName) {
	var=newVar(node, var, fileName);

	controlFlowGraphBlock* whilebody = node->nodes[0];
	var=traverseCfg(whilebody, node, var, fileName);
	controlFlowGraphBlock* exitWhilebody = node->nodes[1];
	var=traverseCfg(exitWhilebody, node, var, fileName);
	node->drawn = 0;
	return var;
}
Node* traverseCfgBaseStatement(controlFlowGraphBlock* node, controlFlowGraphBlock* start, Node * var, char* fileName) {
	var=newVar(node, var, fileName);

	var=traverseCfg(node->nodes[0], node, var, fileName);
	return var;

}

Node* traverseCfg(controlFlowGraphBlock* cfg, controlFlowGraphBlock* start, Node* var, char* fileName) {
	switch (cfg->blocktype)
	{
	case IfBlock:
		var=traverseCfgIfStatement(cfg, start, var, fileName);
		break;
	case WhileBlock:
		if (cfg->drawn == 0) {
			cfg->drawn = 1;
			var=traverseCfgWhileStatement(cfg, start, var, fileName);
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
		var=newVar(cfg, var, fileName);

		return var;
		break;
	case BaseBlock:
		if (cfg->outNodeCount <= 0) {
			var=newVar(cfg, var, fileName);

			return var;
		}
		var=traverseCfgBaseStatement(cfg, start, var, fileName);
		break;
	default:
		if (cfg->blocktype == IfExitBlock) {
			cfg->drawn++;
		}
		if (cfg->blocktype == IfExitBlock && cfg->drawn > 1) {
			break;
		}

		if (cfg->outNodeCount <= 0) {
			var=newVar(cfg, var, fileName);

			return var;
		}
		var=traverseCfgBaseStatement(cfg, start,var , fileName);
		break;
	}
	return var;

}
//TODO: scopes, so that the same variable name can be used by more than one function




