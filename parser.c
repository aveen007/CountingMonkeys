#include <antlr3.h>
#include<antlr3basetree.h>
#include "MyGrammarLexer.h"
#include "MyGrammarParser.h"
#include<antlr3baserecognizer.h>
//#include <antlr3base>

//#include <queue>


#include "parser.h"

#pragma region definitions

const char* getText(pANTLR3_BASE_TREE tree);
pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i);
ErrorInfo* createErrorInfoNode(const char* message);
ErrorInfo* errors;

static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec);
void addError(ErrorInfo** errors, ErrorInfo* newError);
#pragma endregion

ParseResult parse(char* text, size_t size, char* name) {
    FILE* file;
	pANTLR3_INPUT_STREAM inputStream = antlr3StringStreamNew(text, ANTLR3_ENC_UTF8, size, name);
	pMyGrammarLexer l = MyGrammarLexerNew(inputStream);

	pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(l));

	pMyGrammarParser p = MyGrammarParserNew(tstream);
    
    errors = NULL;
    p->pParser->rec->reportError = MyreportError;
	MyGrammarParser_sourcer_return tu = p->sourcer(p);
    
    run2(tu.tree);

    file = fopen("tree.dot", "w");
   
    // Write some text to the file
    fprintf(file, p->adaptor->makeDot(p->adaptor, tu.tree)->chars);

    // Close the file
    fclose(file);
    const char* makeTreeGraph = "dot -Tpng ../cpoCompilerWin/tree.dot -o ../cpoCompilerWin/output.png";
    //const char* makeTreeGraph = "dot -Tpng ./tree.dot -o ./output.png";
    system(makeTreeGraph);
	p->free(p);
	tstream->free(tstream);
	l->free(l);
	inputStream->free(inputStream);
	return (ParseResult) {errors,tu.tree};
}



#pragma region  errors
static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec) {
    rec->state->errorCount++;


    ErrorInfo* newError = createErrorInfoNode(rec->state->exception->message, rec->state->exception->line, rec->state->exception->charPositionInLine);

    addError(&errors, newError);

    return;
}

void addError(ErrorInfo** errors, ErrorInfo* newError) {

    if (*errors == NULL) {
        // If the list is empty, add the new error as the head
        *errors = newError;
    }
    else {
        // Otherwise, append the new error at the end
        ErrorInfo* current = *errors;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newError; // Add the new error to the end
    }

    return;
}
// Function to create a new node
ErrorInfo* createErrorInfoNode(const char* message, int line, int position) {
    ErrorInfo* newNode = (ErrorInfo*)malloc(sizeof(ErrorInfo));
    if (newNode == NULL) {
        // Handle memory allocation failure
        perror("Failed to allocate memory for ErrorInfo");
        exit(EXIT_FAILURE);
    }
    newNode->message = strdup(message); // Duplicate the message string
    newNode->line = line;
    newNode->position = position;
    newNode->next = NULL;
    return newNode;
}
// Function to free the ErrorInfo linked list
void freeErrorInfo(ErrorInfo* head) {
    while (head != NULL) {
        ErrorInfo* temp = head;
        head = head->next;
        free(temp->message); // Free the string
        free(temp);          // Free the node
    }
}
#pragma endregion


void runChildren(pANTLR3_BASE_TREE tree) {
    if (tree->children != NULL) {
        for (int i = 0; i < tree->getChildCount(tree); i++) {
            run2(getChild(tree, i));

        }
    }
    else {
        return 0;
    }
}
int run2(pANTLR3_BASE_TREE tree)
{
    if (tree) {
        pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);


        if (tok) {
            pANTLR3_BASE_TREE full_tree = antlr3BaseTreeNew;
            switch (tok->type) {


            case TypeRef: {
                if (tree->children != NULL) {
                    pANTLR3_BASE_TREE elements = tree;
                    if (tree->children->count > 1) {

                        //to get the type node with its type ref dad
                        pANTLR3_BASE_TREE type_node = getChild(tree, 1);
                        tree->deleteChild(tree, 1);
                        pANTLR3_BASE_TREE type_tree_base = tree;
                         //type_tree_base = tree;

                        pANTLR3_BASE_TREE type_tree = tree->dupNode(tree);
                        type_tree->addChild(type_tree, type_node);
                        ///

                        int complexity = 0;
                        while (tree->getChildCount(tree) > 0) {

                            tree = tree->getChild(tree, 0);
                            pANTLR3_COMMON_TOKEN tok1 = tree->getToken(tree);
                            pANTLR3_COMMON_TOKEN tok3 = elements->getToken(elements);

                            if (tok1->type == Array && tree->getChildCount(tree) == 1) {
                                break;
                            }
                            if (tok1->type == Array && tree->children->count > 1) {
                               
                                complexity++;
                            }

                        }
                  
                        // making the elements node of the last child on the left
                        pANTLR3_BASE_TREE array_tree = tree;
                        array_tree->addChild(array_tree, type_tree);
                        pANTLR3_BASE_TREE elements_tree = getChild(array_tree, 0);
                        array_tree->deleteChild(array_tree, 0);
                        array_tree->addChild(array_tree, elements_tree);
                      


                        //looping to flip the elements order 
                        int complexity2 = complexity;
                        while (complexity2>0){
                        elements = type_tree_base;
                        array_tree = getChild(type_tree_base, 0);
                        
                        
                        
                        while (complexity > -1) {

                            pANTLR3_COMMON_TOKEN tok3 = elements->getToken(elements);

                            if (elements && tok3->type == Elements) {
                                array_tree->addChild(array_tree, elements);

                            }
                            elements = getChild(array_tree, 1);
                            array_tree->deleteChild(array_tree, 1);
                            array_tree = getChild(getChild(array_tree, 0), 0);




                            complexity--;


                        }
                            array_tree = getChild(type_tree_base, 0);
                            array_tree->addChild(array_tree, elements);
                            if (complexity2!=1) {
                                type_tree_base = getChild(getChild(type_tree_base, 0), 0);
                            }
                            complexity2--;
                            complexity = complexity2;

                        }



                        return 0;

                    }
                    runChildren(tree);
                    return 0;
                }

            }

            default: {
                runChildren(tree);
                break;
            }


            }
        }
    }
        return 0;

}
#pragma region unsuccessful attempts




pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    return (pANTLR3_BASE_TREE)tree->children->get(tree->children, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {

    return (const char*)tree->getText(tree)->chars;
}

static void
displayRecognitionErrorNew(pANTLR3_BASE_RECOGNIZER recognizer,
    pANTLR3_UINT8* tokenNames)
{
}
#pragma endregion
