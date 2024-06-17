%{
#include "example.tab.h"
#include <stdio.h>
%}
digit [0-9]
%%
"+" {return ADD;}
"-" {return SUB;}
"*" {return MUL;}
"/" {return DIV;}
{digit}* {
yylval = atoi(yytext);
return INT;
}
%%



