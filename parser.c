#include <antlr3.h>

#include "MyGrammarLexer.h"
#include "MyGrammarParser.h"

#include "parser.h"


ParseResult parse(char* text, size_t size, char* name) {
	pANTLR3_INPUT_STREAM inputStream = antlr3StringStreamNew(text, ANTLR3_ENC_UTF8, size, name);
	
	pMyGrammarLexer l = MyGrammarLexerNew(inputStream);
	pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(l));

	pMyGrammarParser p = MyGrammarParserNew(tstream);

	//pMyGrammarParser_translation_unit_return tu = p->translation_unit(p);

	// tu.tree.

	p->free(p);
	tstream->free(tstream);
	l->free(l);
	inputStream->free(inputStream);

	return (ParseResult) { 0, 0 };
}
