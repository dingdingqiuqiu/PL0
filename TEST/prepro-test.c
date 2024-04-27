
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// 消除注释
/*void removeComments(char *source, char *destination) {
    int i, j;
    int len = strlen(source);
    int isComment = 0; // 用于标记是否处于注释中

    for (i = 0, j = 0; i < len; i++) {
        if (source[i] == '/' && source[i + 1] == '/') {
            isComment = 1; // 单行注释开始
            i++; // 跳过第二个斜杠
        } else if (source[i] == '(' && source[i + 1] == '*') {
            isComment = 1; // 多行注释开始
            i++; // 跳过第二个斜杠
        } else if (source[i] == '*' && source[i + 1] == ')') {
            isComment = 0; // 多行注释结束
            i++; // 跳过第二个斜杠
        } else if (!isComment) {
            destination[j++] = source[i];
        }
    }

    destination[j] = '\0'; // 在目标字符串末尾添加结束符
}*/

void removeComments(char *source, char *destination) {
    int i, j;
    int len = strlen(source);
    int inSingleLineComment = 0;
    int inMultiLineComment = 0;

    for (i = 0, j = 0; i < len; i++) {
        if (!inSingleLineComment && !inMultiLineComment) {
            if (source[i] == '/' && source[i + 1] == '/') {
                inSingleLineComment = 1;
                i++; // 跳过第二个斜杠
            } else if (source[i] == '(' && source[i + 1] == '*') {
                inMultiLineComment = 1;
                i++; // 跳过第二个斜杠
            } else {
                destination[j++] = source[i];
            }
        } else if (inSingleLineComment && source[i] == '\n') {
            inSingleLineComment = 0;
        } else if (inMultiLineComment && source[i] == '*' && source[i + 1] == ')') {
            inMultiLineComment = 0;
            i++; // 跳过第二个斜杠
        }
    }

    destination[j] = '\0'; // 在目标字符串末尾添加结束符
}

int main() {
    char source[] = "//PL/0 demo\n \n(* \n\n\tThis is a\n comment \n*)\n main() {\n\t// This is also a comment\n\treturn 0;\n}";
    char result[1000];

    printf("Original source:\n%s\n\n", source);

    removeComments(source, result);
    printf("After removing comments:\n%s\n\n", result);

    mergeWhitespace(result, result);
    printf("After merging whitespace:\n%s\n\n", result);

    return 0;
}
