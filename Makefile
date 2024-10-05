

.PHONY: build rebuild clean


SRC=./main.c \
	./parser.c \
	./MyGrammarLexer.c \
	./MyGrammarParser.c


build:
	gcc -g  -o ./test $(SRC) -lantlr3c

clean: 
	rm -f ./test

rebuild: clean build
