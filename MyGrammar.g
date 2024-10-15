grammar MyGrammar;

options {
    output=AST;
    language=C;
    backtrack=true;
   memoize=true;
    k=3; 
}
tokens{
Sourcer;
Source;
FuncDef;
FuncSignature;
VarStatement;
IfStatement;
WhileStatement;
DoStatement;
BreakStatement;
Expression;
ArgDef;
ListArgdef;
Element;
TypeRef;
Array;
Builtin;
Custom;
ID;
BuiltinArray;
CustomArray;
ListExpr;

}
sourcer	:	 source  EOF -> ^(Sourcer source);
source: funcDef*  ->^(Source funcDef*);

funcDef: 'function' funcSignature statement* 'end' 'function' -> ^(FuncDef funcSignature statement*);

funcSignature: ID '(' listArgdef? ')' ( 'as' typeRef )? ->^(FuncSignature ^(ID listArgdef?) (typeRef)? );

statement:
    varStatement 
    | ifStatement
    | whileStatement
    | doStatement
    | breakStatement
    | expressionStatement;
varStatement: 'dim' listIdentifier? 'as' element-> ^(VarStatement); // for static typing
ifStatement: 'if' expr 'then' statement* ('else' statement*)? 'end' 'if'-> ^(IfStatement);
whileStatement: 'while' expr statement* 'wend'-> ^(WhileStatement expr statement*);
doStatement: 'do' statement* 'loop' ('while' | 'until') expr->^(DoStatement);
breakStatement: 'break' ->^(BreakStatement);
expressionStatement: expr ';'->^(Expression);

argDef: ID ( 'as' typeRef )? ->^(ArgDef ID (typeRef)?) ;

listArgdef: argDef ( ',' argDef )* ->^(ListArgdef argDef+) ;

element : typeRef | array; // Allows elements to be typeRefs or nested arrays
typeRef : (builtinArray // it could also be customArray
         | customArray)-> ^(TypeRef ^(BuiltinArray builtinArray)? ^(CustomArray customArray)?)
         ;

builtinArray : builtin (array)?->^(Builtin  array?);
customArray  : custom (array)?->^(Custom  array?);

array : '(' (',' )* ')' typeRef* ->^(Array typeRef*);


builtin: 'bool' | 'byte' | 'int' | 'uint' | 'long' | 'ulong' | 'char' | 'string';
custom: ID;

expr 	:	
       (unary   | braces  | place  | atom)(assignmentExpr?) ->^(Expression)
     ;     

assignmentExpr
	:	 (AssignmentOp  |binOp |'(' listExpr? ')') expr ;
	
 AssignmentOp
  :  ('+'|'-'|'*'|'/'|'%'|'<<'|'>>'|'&'|'^'|'|')? '='
  ;    
listExpr:
    expr (',' expr)* ->^(ListExpr expr+);
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
WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') { $channel=HIDDEN; };
fragment Number: '0'..'9';
fragment HexDigit: ('0'..'9'|'a'..'f'|'A'..'F');
fragment Letter: ('a'..'z')|('A'..'Z')|'_';
 PLUS: '+' ;
 MINUS: '-' ;
 MULT: '*' ;
 DIV: '/' ;
 AND: '&&' ;
 OR: '||' ;
 EQ: '==' ;
 NEQ: '!=' ;
 LARGT: '<' ;
 GT: '>' ;
 LEQ: '<=' ;
 GEQ: '>=' ;
fragment Str: '"' (~('"' | '\\'))* '"'; // string surrounded by double quotes
fragment Char: '\'' (  ~('\'' | '\\')) '\''; // single character in single quotes
fragment Hex: '0' ('x' | 'X') HexDigit+; // hexadecimal literal
fragment Bits: '0' ('b' | 'B') ('0' | '1')+; // binary literal
fragment Dec: Number+; // decimal literal
fragment Bool: 'true'|'false';
Literal
  :  Bool
  |  Bits
  |  Hex
  |  Dec
  |  Char
  |  Str
  ;
atom  :  ID|Literal|
  ;
ID: Letter(Letter|Number)*; // identifier

