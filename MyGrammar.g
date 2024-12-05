﻿grammar MyGrammar;

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
ElseStatement;
ThenStatement;
UntilExpr;
ListIdentifier;
Unary;
Braces;
BinaryExpr;
AssignmentExpr;
Elements;
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
varStatement: 'dim' listIdentifier? 'as' element-> ^(VarStatement listIdentifier? element); // for static typing
ifStatement: 'if' expr 'then' statement* elseStatement? 'end' 'if'-> ^(IfStatement ^(Expression expr) ^(ThenStatement statement*) elseStatement?);
elseStatement
	:	 ('else' statement*)->^( ElseStatement statement*);
whileStatement: 'while' expr statement* 'wend'-> ^(WhileStatement ^(Expression expr) statement*);
doStatement: 'do' statement* 'loop' ('while' | 'until') expr->^(DoStatement statement* ^(UntilExpr ^(Expression expr))  );
breakStatement: 'break' ->^(BreakStatement);
expressionStatement: expr ';'->^(Expression expr);

argDef: ID ( 'as' typeRef )? ->^(ArgDef ID (typeRef)?) ;

listArgdef: argDef ( ',' argDef )* ->^(ListArgdef ',' argDef+) ;

element : typeRef | array; 
typeRef : builtin (array )? -> ^(TypeRef array? builtin)
         | custom (array )?->^(TypeRef array?  custom   )
         |array (array )?-> ^(TypeRef  array   array?     )
         ;

array : '(' (',' )* ')' typeRef* ->^(Array ^(typeRef)* ^(Elements  (',')*) );


builtin: 'bool' | 'byte' | 'int' | 'uint' | 'long' | 'ulong' | 'char' | 'string';
custom: ID;

expr 	:	
       (expr0) expr4 ->^(expr4 expr0)
     ; 
expr0	:	(unary   | braces  | place  | atom);
expr1	:	 (assignmentExpr);
expr2	:	 (binaryExpression);
expr3	:	 (listExpr);
expr4	:	(expr1|expr2|expr3)? ;
assignmentExpr
	:	 (AssignmentOp  ) expr -> ^(AssignmentOp expr);

binaryExpression
	:	binOp expr->^(binOp expr);

 AssignmentOp
  :  ('+'|'-'|'*'|'/'|'%'|'<<'|'>>'|'&'|'^'|'|')? '='
  ;    
listExpr:
  '(' ( expr (',' expr)* )?')' expr->^(ListExpr '(' expr+ ')' Expression expr );

listIdentifier:
    (ID (',' ID)*)-> ^(ListIdentifier ID+)
    ;
unary: unOp expr ->^(Unary unOp expr); // where unOp is a unary operator

place: ID ;
literal: Bool|Str|Char|Hex|Bits|Dec;
braces: '(' expr ')'->^(Braces expr);
  

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
    atom  :  ID|Literal|
  ;
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

ID: Letter(Letter|Number)*; // identifier