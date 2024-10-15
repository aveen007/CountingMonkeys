#pragma once

typedef struct ParseTree {
	struct ParseTree* children;
	int childrenCount;
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


