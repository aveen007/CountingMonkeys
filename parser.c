#include <antlr3.h>
#include<antlr3basetree.h>
#include "MyGrammarLexer.h"
#include "MyGrammarParser.h"
#include<antlr3baserecognizer.h>




#include "parser.h"


const char* getText(pANTLR3_BASE_TREE tree);
pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i);
ErrorInfo* createErrorInfoNode(const char* message);
ErrorInfo* errors;

static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec);
void addError(ErrorInfo** errors, ErrorInfo* newError);
ParseResult parse(char* text, size_t size, char* name) {
    FILE* file;
	pANTLR3_INPUT_STREAM inputStream = antlr3StringStreamNew(text, ANTLR3_ENC_UTF8, size, name);
	pMyGrammarLexer l = MyGrammarLexerNew(inputStream);

	pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(l));

	pMyGrammarParser p = MyGrammarParserNew(tstream);
    
    errors = NULL;
    p->pParser->rec->reportError = MyreportError;
	MyGrammarParser_sourcer_return tu = p->sourcer(p);
    

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
static void  MyreportError(pANTLR3_BASE_RECOGNIZER rec) {
    rec->state->errorCount++;


    ErrorInfo *newError = createErrorInfoNode (rec->state->exception->message );
   
    addError(&errors, newError);

    return ;


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
ErrorInfo* createErrorInfoNode(const char* message) {
    ErrorInfo* newNode = (ErrorInfo*)malloc(sizeof(ErrorInfo));
    if (newNode == NULL) {
        // Handle memory allocation failure
        perror("Failed to allocate memory for ErrorInfo");
        exit(EXIT_FAILURE);
    }
    newNode->message = strdup(message); // Duplicate the message string
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

int run(pANTLR3_BASE_TREE tree, pMyGrammarParser p)
{
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (tok) {
        switch (tok->type) {
        case Sourcer: {
            printf(p->adaptor->makeDot(p->adaptor, tree));
            return run(getChild(tree, 0),p) ;
        }
        case Source: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

            return run(getChild(tree, 0), p);
        }
        case FuncDef: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

            return run(getChild(tree, 0),p);


        }
        case FuncSignature: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

            printf(getText(getChild(tree, 0)));
            return run(getChild(tree, 0),p);

        }
        case ListArgdef: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

            return run(getChild(tree, 0),p);

        }
        case ArgDef: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

        }
        case ID: {
            printf(p->adaptor->makeDot(p->adaptor, tree));

        }
        }
    }
    return 0;
     /*   case ID: {
            string var(getText(tree));
            return memory[var];
        }
        case PLUS:
            return run(getChild(tree, 0)) + run(getChild(tree, 1));
        case MINUS:
            return run(getChild(tree, 0)) - run(getChild(tree, 1));
        case TIMES:
            return run(getChild(tree, 0)) * run(getChild(tree, 1));
        case ASSIGN: {
            string var(getText(getChild(tree, 0)));
            int val = run(getChild(tree, 1));
            memory[var] = val;
            return val;
        }
        default:
            cout << "Unhandled token: #" << tok->type << '\n';
            return -1;
        }
    }
    else {
        int k = tree->getChildCount(tree);
        int r = 0;
        for (int i = 0; i < k; i++) {
            r = run(getChild(tree, i));
        }
        return r;
    }*/
}



pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
//    assert(i < tree->getChildCount(tree));
    return (pANTLR3_BASE_TREE)tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {

    return (const char*)tree->getText(tree)->chars;
}

static void
displayRecognitionErrorNew(pANTLR3_BASE_RECOGNIZER recognizer,
    pANTLR3_UINT8* tokenNames)
{
}