#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#define _CRT_SECURE_NO_WARNINGS
char* read_file_to_string(const char* filename);
int main(int argc, char** argv) {
    FILE* file;

    // Open the file for reading


	//char* testText = "function a2 ( a1, a2 as bool (bool)) end function";
    char* testText = read_file_to_string("input.txt");
	ParseResult result = parse(testText, strlen(testText), "test.c");

	ErrorInfo* current = result.errors;
	while (current != NULL) {
		printf("%s", current->message, " at line: ", current->line, " and pos: ", current->position);
			current = current->next;
	}
	freeErrorInfo(result.errors);
    free(testText);
	printf("aveen!\n");
}
char* read_file_to_string(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Could not open file");
        return NULL;
    }

    // Move the file pointer to the end of the file
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file); // Get the size of the file
    fseek(file, 0, SEEK_SET);    // Move the file pointer back to the start of the file

    // Allocate memory for the string, plus one for the null terminator
    char* text = malloc(filesize + 1);
    if (text == NULL) {
        fclose(file);
        perror("Memory allocation failed");
        return NULL;
    }

    // Read the entire file into the text
    size_t bytesRead = fread(text, 1, filesize, file);
    text[bytesRead] = '\0'; // Null-terminate the string

    fclose(file); // Close the file
    return text;
}