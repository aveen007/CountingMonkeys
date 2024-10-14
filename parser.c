#include <antlr3.h>
#include<antlr3basetree.h>
#include "MyGrammarLexer.h"
#include "MyGrammarParser.h"




#include "parser.h"


const char* getText(pANTLR3_BASE_TREE tree);
pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i);
ParseResult parse(char* text, size_t size, char* name) {
	pANTLR3_INPUT_STREAM inputStream = antlr3StringStreamNew(text, ANTLR3_ENC_UTF8, size, name);
	pMyGrammarLexer l = MyGrammarLexerNew(inputStream);

	pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(l));

	pMyGrammarParser p = MyGrammarParserNew(tstream);
    
	MyGrammarParser_sourcer_return tu = p->sourcer(p);
    
    // Open a file in writing mode
    FILE* file = fopen("tree.dot", "w");

    // Write some text to the file
    fprintf(file, p->adaptor->makeDot(p->adaptor, tu.tree)->chars);

    // Close the file
    fclose(file);
	//int rr = run(tu.tree, p);
    //DottreeGenerator 
   // printf("Evaluator result: " + rr + '\n');
    
	p->free(p);
	tstream->free(tstream);
	l->free(l);
	inputStream->free(inputStream);
	return (ParseResult) {0,tu.tree};
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

