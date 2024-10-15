#include <stdio.h>
#include <string.h>

#include "parser.h"

int main(int argc, char** argv) {

	char* testText = "function a2 ( a1,a2 as bool (bool)) end function";

	ParseResult result = parse(testText, strlen(testText), "test.c");

	ErrorInfo* current = result.errors;
	while (current != NULL) {
		printf("%s", current->message);
			current = current->next;
	}
	freeErrorInfo(result.errors);
	printf("aveen!\n");
}
