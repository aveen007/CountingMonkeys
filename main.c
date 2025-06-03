#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "ControlGraph.h"
#include "localVariable.h"
#include "generic.h"
#include <Windows.h>
#include "asm.h"

#define dataAsmOutFilename "../listingData.txt"
#define codeAsmOutFilename "../listingCode.txt"

#define asm_code_header "[section ram, code]\n\tinit CodeEnd\n\tjump main\n"
#define asm_data_header "data:\n"
#define asm_footer "halt:\n\thlt\n"

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

    int numberOfProcedures=0;
    int numberOfFiles = argc-1;
    cfgFile** files=malloc(sizeof(cfgFile*)* numberOfFiles);
    char* name=argv[1];
    for (int i = 0; i < numberOfFiles;i++) {
        files[i] = malloc(sizeof(cfgFile));
        files[i]->name =argv[i+1];
        char* testText = read_file_to_string(files[i]->name);
        ParseResult result = parse(testText, strlen(testText), files[i]->name);
        ParseTree* myTree = result.tree;
    	ErrorInfo* current = result.errors;
        files[i]->ast = result.tree;
	    while (current != NULL) {
	    	printf("%s %s %s %s %s ", current->message, " at line: ", current->line, " and pos: ", current->position);
	    		current = current->next;
	    }
        free(testText);
    }

    subroutineInfo** sub_all_files = malloc(sizeof(Subroutine**) * numberOfFiles);
    classDefInfo* class_all_files = malloc(sizeof(classDefInfo));
    class_all_files->classes = NULL;
    class_all_files->classCount = 0;

    FunctionVariables*** funcVarsInFile = malloc(sizeof(FunctionVariables**)*numberOfFiles);
    for (int i = 0; i < numberOfFiles; i++) {
        numberOfProcedures += files[i]->ast->childrenCount;
        files[i]->cfgs = CFGInterfacer(files[i]->name, files[i]->ast);
        class_all_files->classes = realloc(class_all_files->classes,sizeof(classDef*)* (class_all_files->classCount+ files[i]->cfgs->classes->classCount));
        for (int j = 0; j < files[i]->cfgs->classes->classCount; j++) {
            
            class_all_files->classes[class_all_files->classCount + j] = files[i]->cfgs->classes->classes[j];

        }
        class_all_files->classCount += files[i]->cfgs->classes->classCount;
        subroutineInfo* info= DefineSubprogram(files[i]->name, files[i]->cfgs->cfgs, files[i]->ast);
        sub_all_files[i] = info;
 
        files[i]->cntCfgs = info->count;
        // the function count is not the childeren count 
        //TODO: actually: var tyoes should be set later after substitution

    
   
    }

    for (int i = 1; i < numberOfFiles; i++) {
       
        
       classSubrountineInfo* info =  setTypes(sub_all_files[i]->subroutines, sub_all_files[i]->count, files[i]->name,class_all_files);
       sub_all_files[i]->subroutines = info->subs;
       class_all_files = info->cls;
       funcVarsInFile [i] = getLocalVars(sub_all_files[i]->subroutines, sub_all_files[i]->count , files[i]->name);
    }

    // TODO traverse all Type and set def field

    FILE* data = fopen(dataAsmOutFilename, "w+");
    if (!data) {
        fclose(data);
        printf("The file could not be opened: "dataAsmOutFilename);
        return 1;
    }
    //fprintf(data, "hi");
     FILE* code = fopen(codeAsmOutFilename, "w+");
    if (!code) {
        fclose(code);
        printf("The file could not be opened: "codeAsmOutFilename);
        return 1;
    }
    asmCodeOut = code;
    asmDataOut = data;

    fprintf(asmCodeOut, asm_code_header);

    for (int i = 1; i < numberOfFiles; i++) {
        translate(sub_all_files[i]->subroutines, funcVarsInFile[i], sub_all_files[i]->count, files[i]->name, class_all_files);
    }
    fprintf(asmCodeOut, "\tjump halt\n");
    fprintf(asmDataOut, asm_footer);

    fclose(data);
    char* dataListing = read_file_to_string(dataAsmOutFilename);
    fprintf(data, "\n");
    fprintf(code, dataListing);
    fprintf(asmCodeOut, "CodeEnd:\n");


    fclose(code);
    files = HandleCallGraphs(files, numberOfFiles);
    for (int i = 0; i < numberOfFiles; i++) {
        ErrorInfoCFG* current = files[i]->cfgs->errors;
        while (current != NULL) {
           // printf("%s  \n", current->message);
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
