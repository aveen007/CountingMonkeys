#include <antlr3.h>
#include<antlr3basetree.h>
#include "MyGrammarLexer.h"
#include "MyGrammarParser.h"
#include<antlr3baserecognizer.h>
#include "parser.h"

#pragma region definitions

const char* getText(pANTLR3_BASE_TREE tree);
pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i);
ErrorInfo* createErrorInfoNode(const char* message, int line, int position);


ErrorInfo* errors;
static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec);
void addError(ErrorInfo** errors, ErrorInfo* newError);

ParseTree* CreateMyTreeNode(char* token, int line, int position);
ParseTree* MyTree(pANTLR3_BASE_TREE tree);
void addMyTreeChild(ParseTree* parent, ParseTree* child);
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

    ParseTree * myTree = MyTree(tu.tree);
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
	return (ParseResult) {errors,myTree};
}



#pragma region  errors
static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec) {
    rec->state->errorCount++;


    ErrorInfo* newError = createErrorInfoNode(rec->state->exception->message,(int) rec->state->exception->line,(int) rec->state->exception->charPositionInLine);

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

#pragma region give me tree, give you tree
void addMyTreeChild(ParseTree* parent, ParseTree* child) {
    if (parent == NULL || child == NULL) {
        return; // Ensure parent and child are not NULL
    }

    // Reallocate memory for the children array
    parent->childrenCount++;
    //ParseTree** temp =
    //if (temp == NULL) {

    //    return;
    //}
    //else {
  
    // Step 2: Allocate memory for the new children array
    ParseTree** temp = (ParseTree**)realloc(parent->children,
        parent->childrenCount * sizeof(ParseTree*));

    // Step 3: Handle realloc failure
    if (temp == NULL) {
        // Handle memory allocation failure (possible memory leak if original pointer is not freed)
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE); // or handle according to your application's needs
    }

    // Step 4: Assign the new pointer back to parent->children
    parent->children = temp;

    // Step 5: Assign the new child to the new position in the children array
    parent->children[parent->childrenCount - 1] = child; // Add the new child

}
           
ParseTree* MyTree(pANTLR3_BASE_TREE tree) {
   

        pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);

        int line = (int)tok->line;
        int position = (int)tok->charPosition;
        char *token = (char *)tok->getText(tok)->chars;
     
        //int childCount = ;

        ParseTree* myTree = CreateMyTreeNode(token, line, position);
        if (tree->children != NULL) {
            for (int i = 0; i < tree->children->count; i++) {
               

                addMyTreeChild(myTree, MyTree(getChild(tree, i)));
           
            
            }
        }

            return myTree;
 

}
ParseTree* CreateMyTreeNode( char*token, int line, int position) {
    ParseTree* node = (ParseTree*)malloc(sizeof(ParseTree));
    node->children = NULL;
    node->childrenCount = 0;
    node->token = strdup(token); // Duplicate the token string
    node->line = line;
    node->position = position;
    return node;
}
// Function to free a ParseTree node and its children
void freeParseTree(ParseTree* node) {
    if (node == NULL) {
        return;
    }

    // Recursively free children
    for (int i = 0; i < node->childrenCount; i++) {
        freeParseTree(&node->children[i]);
    }

    free(node->children); // Free the array of children
    free(node->token);    // Free the token string
    free(node);           // Free the node itself
}


#pragma endregion

#pragma region  reconstructing the tree


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





pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    return (pANTLR3_BASE_TREE)tree->children->get(tree->children, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {

    return (const char*)tree->getText(tree)->chars;
}
#pragma endregion
ParseTree* duplicateStruct(const ParseTree* original)

{
    // Allocate memory for the new struct
    ParseTree* copy = (ParseTree*)malloc(sizeof(ParseTree));
    if (copy == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Copy the contents
    memcpy(copy, original, sizeof(ParseTree));

    return copy;
}

