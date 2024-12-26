#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "ControlGraph.h"
#include <Windows.h>
//#define _CRT_SECURE_NO_WARNINGS
char* read_file_to_string(const char* filename);

int numFiles = 0;
char**  list_txt_files(const char* folderPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char** fileNames = malloc(sizeof(char*));

    // Create the search pattern to find .txt files
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.txt", folderPath);

    // Begin searching for files
    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No .txt files found in the specified directory.\n");
        return;
    }
    // Iterate through the found files
    do {
        // Print the name of the file
        numFiles++;
        fileNames = realloc(fileNames, sizeof(char*) * numFiles);
        fileNames[numFiles - 1] = findFileData.cFileName;
        //printf("%s  ", findFileData.cFileName);
    } while (FindNextFile(hFind, &findFileData) != 0);

        // Close the find handle
        FindClose(hFind);
        return fileNames;
}
int main(int argc, char* argv[]) {
    FILE* file;

    // Open the file for reading

    //char** fileNames = list_txt_files("D:\\cpoCompiler\\cpoCompilerWin");
	//char* testText = "function a2 ( a1, a2 as bool (bool)) end function";
    int numberOfProcedures=0;
    int numberOfFiles = argc-1;
    cfgFile** files=malloc(sizeof(cfgFile*)* numberOfFiles);
    char* name=argv[1];
    for (int i = 0; i < numberOfFiles;i++) {
        files[i] = malloc(sizeof(cfgFile));
        //size_t filename_size = (size_t)snprintf(NULL, 0,argv[i]);
        //files[i]->name = malloc(sizeof(char) * filename_size);
        files[i]->name =argv[i+1];
        //snprintf(files[0]->name, filename_size + 1, argv[i]);
        char* testText = read_file_to_string(files[i]->name);
        ParseResult result = parse(testText, strlen(testText), files[i]->name);
        ParseTree* myTree = result.tree->children[0]->children[0];
    	ErrorInfo* current = result.errors;
        files[i]->ast = result.tree;
	    while (current != NULL) {
	    	printf("%s %s %s %s %s ", current->message, " at line: ", current->line, " and pos: ", current->position);
	    		current = current->next;
	    }
        free(testText);
    }
    //callGraph** CGs=malloc(sizeof(callGraph*));
    //CfgsInfo** correspondingCFGs =createCfgsInfo();
    for (int i = 0; i < numberOfFiles; i++) {
        numberOfProcedures += files[i]->ast->children[0]->childrenCount;
        //CGs = realloc(CGs,sizeof(callGraph*) * numberOfProcedures);
        files[i]->cfgs = CFGInterfacer(files[i]->name, files[i]->ast, 0);

    Subroutine** subroutines = DefineSubprogram(files[i]->name,files[i]->cfgs->cfgs, files[i]->ast);
   /* free(files[i]->ast);
    free(files[i]->name);*/
    //free(files);
    
     
    }
    files = HandleCallGraphs(files, numberOfFiles);
    for (int i = 0; i < numberOfFiles; i++) {
        ErrorInfoCFG* current = files[i]->cfgs->errors;
        while (current != NULL) {
            printf("%s  ", current->message);
            //printf("%s %s %d %s %d ", current->message, " at line: ", current->line, " and pos: ", current->position);
            break;
            if (current->next != NULL) {
                current = current->next;

            }
            else {
                break;
            }
        }
    
    }
         printCallGraph(files, numberOfFiles);

    //free(files);


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