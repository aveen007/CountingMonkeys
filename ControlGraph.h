#pragma once
#include "parser.h"
typedef struct ArgumentDef {

	char* name;
	char* type;
}ArgumentDef;
typedef struct Position {
	int line;
	int width;
}Position;
typedef struct SignatureDetails {

	char* name;
	struct ArgumentDef* arguments;
	char* returnType;
	struct Position position;

}SignatureDetails;
enum BlockType
{
	BaseBlock, WhileBlock, IfBlock, DoWhileBlock, BreakBlock, ExpressionBlock

};
typedef struct controlFlowGraphBlock{
	enum BlockType blocktype;
	struct controlFlowGraphBlock* outNodes;
	struct controlFlowGraphBlock* inNodes;

}controlFlowGraphBlock;
typedef struct Subroutine {
	char* name;
	struct controlFlowGraphBlock* cfg;
	struct SignatureDetails* signatureDetails;

}Subroutine;
Subroutine CreateCFG(char* fileName, ParseTree *tree);