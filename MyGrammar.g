grammar MyGrammar;

options {
    output=AST;
    language=C;
    backtrack=true;
   memoize=true;
    k=3; 
}
tokens{
Source;
FuncDef;
FuncSignature;
Var;
If;
While;
Do;
Break;
Expression;
Argdef;
List_Argdef;
Element;
TypeRef;
Array;
Builtin;
Custom;

}

source: funcDef*;

funcDef: 'function' funcSignature statement* 'end' 'function';

funcSignature: ID '(' listArgdef? ')' ( 'as' typeRef )?;

statement:
    varStatement
    | ifStatement
    | whileStatement
    | doStatement
    | breakStatement
    | expressionStatement;
varStatement: 'dim' listIdentifier? 'as' element; // for static typing
ifStatement: 'if' expr 'then' statement* ('else' statement*)? 'end' 'if';
whileStatement: 'while' expr statement* 'wend';
doStatement: 'do' statement* 'loop' ('while' | 'until') expr;
breakStatement: 'break';
expressionStatement: expr ';';

argDef: ID ( 'as' typeRef )?;

listArgdef: argDef ( ',' argDef )*;

element : typeRef | array; // Allows elements to be typeRefs or nested arrays
typeRef : builtinArray // it could also be customArray
         | customArray
         ;

builtinArray : builtin (array);
customArray  : custom array;

array : '(' (element (',' element)*)? ')' ;


builtin: 'bool' | 'byte' | 'int' | 'uint' | 'long' | 'ulong' | 'char' | 'string';
custom: ID;

expr 	:	
       unary   lexpr
       | braces  lexpr
       | place  lexpr
       | literal  lexpr
     ;     
lexpr :  (binOp expr lexpr)
       | ('(' listExpr? ')' lexpr)  |  
        ;
listExpr:
    expr (',' expr)*;
listIdentifier:
    (ID (',' ID)*)
    ;
unary: unOp expr; // where unOp is a unary operator

place: ID;
literal: Bool|Str|Char|Hex|Bits|Dec;
braces: '(' expr ')';
  

unOp:
    '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | '!';
// Define binary operators
binOp:
    PLUS   
    | MINUS   
    | MULT    
    | DIV      
    | AND      
    | OR     
    | EQ      
    | NEQ      
    | LARGT       
    | GT       
    | LEQ      
    | GEQ;
// Lexical tokens
WS : ' ' | '\n' { $channel=HIDDEN; };
fragment Number: '0'..'9';
fragment HexDigit: ('0'..'9'|'a'..'f'|'A'..'F');
fragment Letter: ('a'..'z')|('A'..'Z')|'_';
fragment PLUS: '+' ;
fragment MINUS: '-' ;
fragment MULT: '*' ;
fragment DIV: '/' ;
fragment AND: '&&' ;
fragment OR: '||' ;
fragment EQ: '==' ;
fragment NEQ: '!=' ;
fragment LARGT: '<' ;
fragment GT: '>' ;
fragment LEQ: '<=' ;
fragment GEQ: '>=' ;
fragment Str: '"' (~('"' | '\\'))* '"'; // string surrounded by double quotes
fragment Char: '\'' (EscapeSequence | ~('\'' | '\\')) '\''; // single character in single quotes
fragment Hex: '0' ('x' | 'X') HexDigit+; // hexadecimal literal
fragment Bits: '0' ('b' | 'B') ('0' | '1')+; // binary literal
fragment Dec: Number+; // decimal literal
fragment Bool: 'true'|'false';
ID: Letter(Letter|Number)*; // identifier
EscapeSequence: '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\');
OctalEscape: '\\' ('0'..'3') ('0'..'7') ('0'..'7') | '\\' ('0'..'7') ('0'..'7') | '\\' ('0'..'7');
