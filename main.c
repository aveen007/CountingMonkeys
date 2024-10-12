#include <stdio.h>
#include <string.h>

#include "parser.h"

int main(int argc, char** argv) {

	char* testText = " function a1 ( a1 ) end function";

	ParseResult result = parse(testText, strlen(testText), "test.c");



	printf("aveen!\n");
}
