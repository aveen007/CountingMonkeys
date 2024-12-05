#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "ControlGraph.h"
//#define _CRT_SECURE_NO_WARNINGS
char* read_file_to_string(const char* filename);
int main(int argc, char** argv) {
    FILE* file;

    // Open the file for reading


	//char* testText = "function a2 ( a1, a2 as bool (bool)) end function";
    int numberOfFiles = 1;
    cfgFile** files=malloc(sizeof(cfgFile*)* numberOfFiles);
    for (int i = 0; i < numberOfFiles;i++) {
        files[i] = malloc(sizeof(cfgFile));
        size_t filename_size = (size_t)snprintf(NULL, 0,"input.txt");
        files[i]->name = malloc(filename_size + 1);
        snprintf(files[0]->name, filename_size + 1, "input.txt");
        char* testText = read_file_to_string(files[0]->name);
        ParseResult result = parse(testText, strlen(testText), files[i]->name);
        ParseTree* myTree = result.tree->children[0]->children[0];
    	ErrorInfo* current = result.errors;
        files[0]->ast = result.tree;
	    while (current != NULL) {
	    	printf("%s %s %s %s %s ", current->message, " at line: ", current->line, " and pos: ", current->position);
	    		current = current->next;
	    }
        free(testText);
    }
    for (int i = 0; i < numberOfFiles; i++) {
  
    CfgsInfo*  correspondingCFGs = CFGInterfacer(files[i]->name, files[i]->ast);
    Subroutine** subroutines = DefineSubprogram(files[i]->name,correspondingCFGs->cfgs, files[i]->ast);
    free(files[i]->ast);
    free(files[i]->name);
    free(files[i]);
    }
    free(files);


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