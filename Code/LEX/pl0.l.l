%{
#include <stdio.h>      // for input && output
#include <string.h>     // for String
#include <stdbool.h>    // for bool
#include <stdlib.h>     // for free && malloc

const char *categoryStrings[] = {
    "IDENTIFIER",    // 标识符
    "CONSTANT",      // 常数
    "OPERATOR_ADD",  // 加法运算符 '+'
    "OPERATOR_SUB",  // 减法运算符 '-'
    "OPERATOR_MUL",  // 乘法运算符 '*'
    "OPERATOR_DIV",  // 除法运算符 '/'
    "OPERATOR_EQ",   // 等号 '='
    "OPERATOR_NE",   // 不等号 '#'
    "OPERATOR_LT",   // 小于 '<'
    "OPERATOR_GT",   // 大于 '>'
    "OPERATOR_ASSIGN", // 赋值运算符 ':='
    "DELIMITER_LPAREN", // 左括号 '('
    "DELIMITER_RPAREN", // 右括号 ')'
    "DELIMITER_COMMA",  // 逗号 ','
    "DELIMITER_PERIOD", // 句号 '.'
    "DELIMITER_SEMICOLON", // 分号 ';'
    "KEYWORD_BEGIN",   // begin
    "KEYWORD_END",     // end
    "KEYWORD_IF",      // if
    "KEYWORD_THEN",    // then
    "KEYWORD_WHILE",   // while
    "KEYWORD_DO",      // do
    "KEYWORD_CONST",   // const
    "KEYWORD_VAR",     // var
    "KEYWORD_CALL",    // call
    "KEYWORD_PROCEDURE", // procedure
    "KEYWORD_ODD"      // odd
};

// 定义单词种别
typedef enum {
    IDENTIFIER,       // 标识符
    CONSTANT,         // 常数
    OPERATOR_ADD,     // 加法运算符 '+'
    OPERATOR_SUB,     // 减法运算符 '-'
    OPERATOR_MUL,     // 乘法运算符 '*'
    OPERATOR_DIV,     // 除法运算符 '/'
    OPERATOR_EQ,      // 等号 '='
    OPERATOR_NE,      // 不等号 '#'
    OPERATOR_LT,      // 小于 '<'
    OPERATOR_GT,      // 大于 '>'
    OPERATOR_ASSIGN,  // 赋值运算符 ':='
    DELIMITER_LPAREN, // 左括号 '('
    DELIMITER_RPAREN, // 右括号 ')'
    DELIMITER_COMMA,  // 逗号 ','
    DELIMITER_PERIOD, // 句号 '.'
    DELIMITER_SEMICOLON, // 分号 ';'
    KEYWORD_BEGIN,    // begin
    KEYWORD_END,      // end
    KEYWORD_IF,       // if
    KEYWORD_THEN,     // then
    KEYWORD_WHILE,    // while
    KEYWORD_DO,       // do
    KEYWORD_CONST,    // const
    KEYWORD_VAR,      // var
    KEYWORD_CALL,     // call
    KEYWORD_PROCEDURE,// procedure
    KEYWORD_ODD       // odd
} Category;

typedef struct {
    Category category;  // 单词的种别
    int attribute;      // 属性值，可以根据需要选择合适的数据类型
} Token;
Token token;
char *identifierStrings[1000]; // 标识符表
int identifierId;               //当前标识符表长度
void printToken(Token token);
Category stringToCategory(const char *);

%}
BEGIN   begin
END     end
IF      if
THEN    then
WHILE   while
DO      do
CONST   const
VAR     var
CALL    call
PROCEDURE procedure
ODD     odd
letter [a-zA-Z]
digit  [0-9]
PLUS   "+"
MINUS  "-"
STAR   "*"
DIV    "/"
EQ     "="
NOTEQ  "#"
LT     "<"
GT     ">"
ASSIGN ":="
LP      \(
RP      \)
COMMA   ","
DOT     "."
COLON   ";"
COMMENT "//"
SPACE   [\r\n\t" "]+
ID      [A-Za-z_][A-Za-z_0-9]*
%option noyywrap
%%
\/\/.*  {

}
\(\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+\) {

}

{PLUS}  {
    token.category = stringToCategory(categoryStrings[2]);
    token.attribute = -1;
    printToken(token);
} 
{MINUS} {
    token.category = stringToCategory(categoryStrings[3]);
    token.attribute = -1;
    printToken(token);
}
{STAR}  {
    token.category = stringToCategory(categoryStrings[4]);
    token.attribute = -1;
    printToken(token);
}
{DIV}   {
    token.category = stringToCategory(categoryStrings[5]);
    token.attribute = -1;
    printToken(token);
}
{EQ}    {
    token.category = stringToCategory(categoryStrings[6]);
    token.attribute = -1;
    printToken(token);
}
{NOTEQ}     {
    token.category = stringToCategory(categoryStrings[7]);
    token.attribute = -1;
    printToken(token);
}
{LT}    {
    token.category = stringToCategory(categoryStrings[8]);
    token.attribute = -1;
    printToken(token);
}
{GT}    {
    token.category = stringToCategory(categoryStrings[9]);
    token.attribute = -1;
    printToken(token);
}
{ASSIGN} {
    token.category = stringToCategory(categoryStrings[10]);
    token.attribute = -1;
    printToken(token);
}
{LP}    {
    token.category = stringToCategory(categoryStrings[11]);
    token.attribute = -1;
    printToken(token);
}
{RP}    {
    token.category = stringToCategory(categoryStrings[12]);
    token.attribute = -1;
    printToken(token);
}
{COMMA} {
    token.category = stringToCategory(categoryStrings[13]);
    token.attribute = -1;
    printToken(token);
}
{}
{DOT}   {
    token.category = stringToCategory(categoryStrings[14]);
    token.attribute = -1;
    printToken(token);
}
{COLON} {
    token.category = stringToCategory(categoryStrings[15]);
    token.attribute = -1;
    printToken(token);
}
{digit}+    {
    token.category = CONSTANT;
    token.attribute = atoi(yytext);
    printToken(token);
}
{BEGIN}     {
    token.category = stringToCategory(categoryStrings[16]);
    token.attribute = -1;
    printToken(token);
}
{END}       {
    token.category = stringToCategory(categoryStrings[17]);
    token.attribute = -1;
    printToken(token);
}
{IF}        {
    token.category = stringToCategory(categoryStrings[18]);
    token.attribute = -1;
    printToken(token);
}
{THEN}      {
    token.category = stringToCategory(categoryStrings[19]);
    token.attribute = -1;
    printToken(token);
}
{WHILE}     {
    token.category = stringToCategory(categoryStrings[20]);
    token.attribute = -1;
    printToken(token);
}
{DO}        {
    token.category = stringToCategory(categoryStrings[21]);
    token.attribute = -1;
    printToken(token);
}
{CONST}     {
    token.category = stringToCategory(categoryStrings[22]);
    token.attribute = -1;
    printToken(token);
}
{VAR}       {
    token.category = stringToCategory(categoryStrings[23]);
    token.attribute = -1;
    printToken(token);
}
{CALL}      {
    token.category = stringToCategory(categoryStrings[24]);
    token.attribute = -1;
    printToken(token); 
}
{PROCEDURE} {
    token.category = stringToCategory(categoryStrings[25]);
    token.attribute = -1;
    printToken(token);
}
{ODD}       {
    token.category = stringToCategory(categoryStrings[26]);
    token.attribute = -1;
    printToken(token);
}
{ID}    {
    token.category = 0;
    bool isHadIden = false;
    int numIdentifier = sizeof(identifierStrings) / sizeof(identifierStrings[0]);
    int j = 0;
    for (; j < numIdentifier; j++) {
        if (strcmp(yytext, identifierStrings[j]) == 0) {
            isHadIden = true;
            break;
        }
    }
    if (isHadIden) {
        token.attribute = j;
    } else {
        //identifierStrings[identifierId] = buffer; // BUG!!!
        strcpy(identifierStrings[identifierId],yytext);
        token.attribute = identifierId++;
    }
    printToken(token);  
}
{SPACE}    {

}
%%

Category stringToCategory(const char *str) {
    for (int i = 0; i < sizeof(categoryStrings) / sizeof(categoryStrings[0]); ++i) {
        if (strcmp(str, categoryStrings[i]) == 0) {
            return (Category)i;
        }
    }
    // 如果未找到匹配的字符串，则返回一个默认值，比如IDENTIFIER
    return IDENTIFIER;
}


void printToken(Token token) {
    if (token.category == 0 || token.category == 1) {
        printf("(%s, %d)\n", categoryStrings[token.category], token.attribute);
    } else {
        printf("(%s, NULL)\n", categoryStrings[token.category]); 
    }
}

int main(int argc, char** argv) {
    // 为指针数组中每个指针分配空间
    // 这个指针数据将会被用于标识符表的构建
     for (int i = 0; i < 1000; ++i) {
        // 为每个指针分配内存
        identifierStrings[i] = (char *)malloc(100 * sizeof(char)); // 假设每个字符串最大长度为 100
    }
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    while (yylex() != 0);
    printf("标识符表如下:\n");
    for(int i = 0;i < identifierId;i++) {
        puts(identifierStrings[i]);
    }
    // 释放为指针数组identifierStrings（标识符表）分配的空间
    for(int i = 0;i < 1000;i++) {
        free(identifierStrings[i]);
    }

    return 0;
}
