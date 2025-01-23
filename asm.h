#pragma once
#include "ControlGraph.h"
#include "localVariable.h"
extern int labelCounter;
extern FILE* asmCodeOut;
extern FILE* asmDataOut;
int generateAsm(Node* localVars);
typedef struct Data {
	Type type;
	void* value;
}Data;
typedef struct Var {
	 Data* data;
	char* id;
}Var;