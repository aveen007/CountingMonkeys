#pragma once

//typedef struct ParseTreeNode {
//
//	char* token;
//	int line;
//	int position;
//}ParseTreeNode, *pParseTreeNode;
typedef struct ParseTree {
	 struct ParseTree **  children;
	int childrenCount;
		char* token;
	int line;
	int position;
} ParseTree;

typedef struct ErrorInfo {
	char* message;
	int line;
	int position;
	struct ErrorInfo* next;
} ErrorInfo;

typedef struct ParseResult {
	ErrorInfo* errors;
	ParseTree* tree;
} ParseResult;

ParseResult parse(char* text, size_t size, char* name);
void freeErrorInfo(ErrorInfo* head);
ParseTree* duplicateStruct(const ParseTree* original);
