#pragma once
#include "ControlGraph.h"

typedef struct Data {
	Type type;
	void* value;
}Data;
typedef struct Var {
	 Data* data;
	char* id;
}Var;