%{
#include <stdio.h>
%}

// 大写字母文法终结符号
// 小写字母非终结符
// %token定义token
// 这里是对终结符的定义
// 未被定义的均为非终结符
%token INT
%token ADD SUB MUL DIV
%left ADD SUB
%left MUL DIV
// 这里放下面的优先级更高

%% 
/* 解析规则 */
// 计算是输出expr
calculation: | expr {printf("Result:%d\n",$1);};

// expr的计算
expr: expr ADD expr{$$ = $1 + $3;}
    | expr SUB expr{$$ = $1 - $3;}
    | expr MUL expr{$$ = $1 * $3;}
    | expr DIV expr{$$ = $1 / $3;}
    | '(' expr ')'{$$ = $2;}
    | INT      {$$ = $1;}
;
%%
#include "lex.yy.c"
int main() {
    yyparse();
}
int yyerror(char* msg) {
    fprintf(stderr,"error:%s\n",msg);
} 

