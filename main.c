#include <stdio.h>
#include <string.h>

#include "parser.h"

int main(int argc, char** argv) {

	char* testText = " int test(int x) { return x; } \n\n";

	ParseResult result = parse(testText, strlen(testText), "test.c");



	printf("aveen!\n");
}
