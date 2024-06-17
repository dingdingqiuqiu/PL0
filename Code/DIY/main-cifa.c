#include <stdio.h>      // for input && output
#include <string.h>     // for String
#include <stdbool.h>    // for bool
#include <stdlib.h>     // for free && malloc
#define MAXFILENAME 100
#define BUFFERSIZE 1024

int inSingleLineComment = 0;
int inMultiLineComment = 0;

char ch;             // 当前读取的字符
int identifierId = 0;    // 标识符表中的位置
char *identifierStrings[1000]; // 标识符表 

// 定义单词种别的字符串表示数组
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

// 定义单词符号结构体
typedef struct {
    int category;  // 单词的种别
    int attribute;      // 属性值，可以根据需要选择合适的数据类型
} Token;

// 定义打印单词符号的函数
/*void printToken(Token token) {
    printf("(%d, %d)\n", token.category, token.attribute);
}*/

// 定义打印单词符号的函数
void printToken(Token token) {
    if (token.category == 0 || token.category == 1) {
        printf("(%s, %d)\n", categoryStrings[token.category], token.attribute);
    } else {
        printf("(%s, NULL)\n", categoryStrings[token.category]); 
    }
}

//是否为数字
bool isDigit(char c) {
    bool flag = false;
    if (c >= '0' && c <= '9') {
        flag = true;
    }
    return flag;
}

// 是否为字母
bool isLetter(char c) {
    bool flag = false;
    if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') {
        flag = true;
    }
    return flag;
}

Category stringToCategory(const char *str) {
    for (int i = 0; i < sizeof(categoryStrings) / sizeof(categoryStrings[0]); ++i) {
        if (strcmp(str, categoryStrings[i]) == 0) {
            return (Category)i;
        }
    }
    // 如果未找到匹配的字符串，则返回一个默认值，比如IDENTIFIER
    return IDENTIFIER;
}

//词法分析，得到单词符号
//如果分析结束，返回NULL
// getToken 函数：从文件中读取下一个 Token
Token getToken(FILE *fp) {
    Token token;

    // 跳过空白字符
    do {
        ch = fgetc(fp);
    } while(ch == ' ' || ch == 10 || ch == 9 || ch == 13);

    // 根据读取到的字符判断 Token 类型
    switch (ch) {
        case EOF:
            token.category = -1;
            break;
        case '+':
            token.category = stringToCategory(categoryStrings[2]);
            break;
        case '-':
            token.category = stringToCategory(categoryStrings[3]);
            break;
        case '*':
            token.category = stringToCategory(categoryStrings[4]);
            break; 
        case '/':
            token.category = stringToCategory(categoryStrings[5]);
            break;
        case '=':
            token.category = stringToCategory(categoryStrings[6]);
            break;
        case '#':
            token.category = stringToCategory(categoryStrings[7]);
            break;
        case '<':
            token.category = stringToCategory(categoryStrings[8]);
            break;
        case '>':
            token.category = stringToCategory(categoryStrings[9]);
            break;
        case ':':
            if (ch = fgetc(fp) == '=')
            token.category = stringToCategory(categoryStrings[10]);
            break;
        case '(':
            token.category = stringToCategory(categoryStrings[11]);
            break;
        case ')':
            token.category = stringToCategory(categoryStrings[12]);
            break;
        case ',':
            token.category = stringToCategory(categoryStrings[13]);
            break;
        case '.':
            token.category = stringToCategory(categoryStrings[14]);
            break;
        case ';':
            token.category = stringToCategory(categoryStrings[15]);
            break;
        default:
            // 如果读取到的是数字，则继续读取直到非数字字符，并将其转换为整数
            if (isDigit(ch)) {
                token.category = stringToCategory(categoryStrings[CONSTANT]); // toekn类型为常数
                ungetc(ch, fp); // 将读取到的数字字符放回流中
                fscanf(fp, "%d", &token.attribute); // 将数字值放入属性值
            } else if (isLetter(ch)) {
                // 如果读取到的字符是字母
                // 有可能为IDENTIFIER
                // 有可能为KEYWORD
                // 思路是整个缓冲区，往缓冲区里写单词
                // 再对比和关键词表中的关键词是否一致
                // 如果不一致，铁标识符
                // 标识符只能是字母，最大长度为10
                char buffer[100]; // 假设标识符或关键字的最大长度为 100
                int i = 0;
                buffer[i++] = ch;

                // BUG
                // while ((ch = fgetc(fp)) != EOF && isLetter(ch = fgetc(fp))) {
                while ((ch = fgetc(fp)) != EOF && (isLetter(ch) || isDigit(ch))) {
                    buffer[i++] = ch;
                }
                ungetc(ch,fp); // 将读取到的非字母字符放回去
                buffer[i] = '\0';
                //puts(buffer);
                //token.category = -1;
                //return token;
                 // 检查是否是关键字
                // 假设有一个关键字列表
                char *keywords[] = {"begin", "end", "if", "then", "while", "do", "const", "var", "call" , "procedure", "odd"}; // 关键字表
                int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
                for (int j = 0; j < numKeywords; j++) {
                    if (strcmp(buffer, keywords[j]) == 0) {
                        token.category = stringToCategory(categoryStrings[16+j]);
                        return token;
                    }
                }

                // 不是关键字则为标识符
                // 不过应该先查找原标识符表中是否存在该word
                // 不存在再id++
                // 这块出了BUG
                token.category = stringToCategory("IDENTIFIER");
                bool isHadIden = false;
                int numIdentifier = sizeof(identifierStrings) / sizeof(identifierStrings[0]);
                int j = 0;
                for (; j < numIdentifier; j++) {
                    if (strcmp(buffer, identifierStrings[j]) == 0) {
                        isHadIden = true;
                        break;
                    }
                }
                if (isHadIden) {
                    token.attribute = j;
                } else {
                    //identifierStrings[identifierId] = buffer; // BUG!!!
                    strcpy(identifierStrings[identifierId],buffer);
                    token.attribute = identifierId++;
                }
            }
    }

    return token;
}


// 消除注释
void removeComments(char *source, char *destination) {
    int i, j;
    int len = strlen(source);

    for (i = 0, j = 0; i < len; i++) {
        if (!inSingleLineComment && !inMultiLineComment) {
            if (source[i] == '/' && source[i + 1] == '/') {
                inSingleLineComment = 1;
                i++; // 跳过第二个斜杠
            } else if (source[i] == '(' && source[i + 1] == '*') {
                inMultiLineComment = 1;
                i++; // 跳过第二个星号
            } else {
                destination[j++] = source[i];
            }
        } else if (inSingleLineComment && source[i] == '\n') {
            inSingleLineComment = 0;
        } else if (inMultiLineComment && source[i] == '*' && source[i + 1] == ')') {
            inMultiLineComment = 0;
            i++; // 跳过第二个括号
        }
    }

    destination[j] = '\0'; // 在目标字符串末尾添加结束符
}

// 合并空白符
void mergeWhitespace(char *source, char *destination) {
    int i, j;
    int len = strlen(source);
    int isWhitespace = 0; // 用于标记前一个字符是否是空白符

    for (i = 0, j = 0; i < len; i++) {
        if (source[i] == ' ' || source[i] == '\t' || source[i] == '\n' || source[i] == '\r') {
            if (!isWhitespace) {
                destination[j++] = ' ';
                isWhitespace = 1;
            }
        } else {
            destination[j++] = source[i];
            isWhitespace = 0;
        }
    }

    destination[j] = '\0'; // 在目标字符串末尾添加结束符
}


int main(int argc, char *argv[]) {
    // 为指针数组中每个指针分配空间
    // 这个指针数据将会被用于标识符表的构建
     for (int i = 0; i < 1000; ++i) {
        // 为每个指针分配内存
        identifierStrings[i] = (char *)malloc(100 * sizeof(char)); // 假设每个字符串最大长度为 100
    }
    //得到文件名
    FILE *file;
    FILE *preFile;
    char fileName[MAXFILENAME];
    char preFileName[MAXFILENAME+2];
    if(argc > 1) {
        strcpy(fileName, argv[1]);
    } else {
        printf("请输入要分析的文件名:\n");
        scanf("%99s",fileName);
    }
    //puts(fileName);
    //printf("%s",fileName);

    //打开获取的文件
    file = fopen(fileName,"rw");
    if(file == NULL) {
        printf("无法打开文件 %s\n",fileName);
        return 1;
    }

    //构造输出文件名
    char *lastDot = strrchr(fileName, '.');
    if(lastDot != NULL) {
        // 将最后一个点以及后面的内容替换为".i"
        sprintf(preFileName, "%.*s.i", (int)(lastDot - fileName), fileName);
    } else {
        //没找到直接追加".i"
        sprintf(preFileName, "%s.i", fileName);
    }
    //puts(preFileName);

    //打开输出文件
    preFile = fopen(preFileName,"w");
    //preFile =fopen("11","w");
    //灵异事件，不知道为什么执行过一遍这条语句后，所有文件均能以写入打开了！？
    if (preFile == NULL) {
        printf("无法创建输出文件 %s\n",preFileName);
        fclose(file);
        return 1;
    }
    //printf("成功创建了文件");

    //执行不断从文件中读入数据到内存中处理，预处理的主要步骤
    //主要进行注释删除以及空格合并
    //这里fgets函数是整行读取，与注释格式冲突，出现bug
    /*char inputBuffer[1024],outputBuffer[1024];
    int BUFFER_SIZE = 1024;
    while (fgets(inputBuffer, BUFFER_SIZE, file) != NULL) {
        removeComments(inputBuffer,outputBuffer);
        mergeWhitespace(outputBuffer,outputBuffer);
        fputs(outputBuffer, preFile);
    }*/
    printf("开始预处理\n");
    char inputBuffer[BUFFERSIZE],outputBuffer[BUFFERSIZE];
    char ch;
    int index = 0;
    while (ch = fgetc(file)) {
        if (index == BUFFERSIZE) {
            index = 0;
            removeComments(inputBuffer,outputBuffer);
            mergeWhitespace(outputBuffer,outputBuffer);
            fputs(outputBuffer,preFile);
        } else if (ch == EOF){
            inputBuffer[index] = '\0'; // 好悬没给我整个大BUG
            index = 0;
            removeComments(inputBuffer,outputBuffer);
            mergeWhitespace(outputBuffer,outputBuffer);
            fputs(outputBuffer,preFile);
            //puts(inputBuffer);
            //puts(outputBuffer);
            break;
        }else {
            inputBuffer[index++] = ch;
        }
    } 

    
     //关闭文件流
    fclose(file);
    fclose(preFile);

    //打印预处理的结果
    printf("预处理完毕\n");
    printf("文件%s的内容如下:\n",preFileName);
    FILE * result = fopen(preFileName,"r");
    while ((ch = fgetc(result)) != EOF && index < BUFFERSIZE - 1) {
        inputBuffer[index++] = ch;
    }
    inputBuffer[index] = '\0'; // 在末尾添加字符串结束符
    printf("%s\n", inputBuffer);
    fclose(result);
    //开始词法分析
    printf("开始词法分析\n");

    preFile = fopen(preFileName,"r");
    //preFile = fopen("11","r");

    //灵异事件，不知道为什么执行过一遍这条语句后，所有文件均能以写入打开了！？
    if (preFile == NULL) {
        printf("无法打开待分析文件 %s\n",preFileName);
        return 1;
    }
    printf("成功打开待分析文件\n");
    printf("分析结果如下:\n");
    
    Token token;
    token = getToken(preFile);
    printToken(token);
    //printToken(token);
    while ((token = getToken(preFile)).category != -1) {
        //打印验证
        printToken(token);
    }
    //打印标识符表
    printf("标识符表如下:\n");
    for(int i = 0;i < identifierId;i++) {
        puts(identifierStrings[i]);
    }

    //关闭文件读取流
    fclose(preFile);
    // 释放为指针数组identifierStrings（标识符表）分配的空间
    for(int i = 0;i < 1000;i++) {
        free(identifierStrings[i]);
    }
    return 0;
}
