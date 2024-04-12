#include <stdio.h>
#include <string.h>
#define MAXFILENAME 100
#define BUFFERSIZE 1024

int inSingleLineComment = 0;
int inMultiLineComment = 0;

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
    //preFile =
    //fopen("11","w");灵异事件，不知道为什么执行过一遍这条语句后，所有文件均能以写入打开了！？
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
            index = 0;
            removeComments(inputBuffer,outputBuffer);
            mergeWhitespace(outputBuffer,outputBuffer);
            fputs(outputBuffer,preFile);
            break;
        }else {
            inputBuffer[index++] = ch;
        }
    } 

    //关闭文件流
    fclose(file);
    fclose(preFile);
    //printf("预处理完成,处理结果已写入文件 %s\n",preFileName);

    return 0;
}
