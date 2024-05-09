%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <string.h>
    #include "syntax.tab.h"
    #include <node.h>
    extern int errorNum;
%}
%option yylineno

SEMI ;
COMMA ,
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"

