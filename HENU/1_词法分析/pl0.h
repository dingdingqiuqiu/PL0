#include <stdio.h>

#define norw        11             // 保留字的数量
#define txmax       100            // 标识符表的长度
#define nmax        14             // 允许的最长数字位数
#define al          10             // 标识符的最大长度
#define amax        2047           // 寻址空间
#define levmax      3              // 最大嵌套层数
#define cxmax       2000           // 代码数组的长度

#define nul         0x1            // 空
#define ident       0x2            // 标识符
#define number      0x4            // 数值
#define plus        0x8            // +
#define minus       0x10           // -
#define times       0x20           // *
#define slash       0x40           // /
#define oddsym      0x80           // 奇数判断
#define eql         0x100          // =
#define neq         0x200          // <>
#define lss         0x400          // <
#define leq         0x800          // <=
#define gtr         0x1000         // >
#define geq         0x2000         // >=
#define lparen      0x4000         // (
#define rparen      0x8000         // )
#define comma       0x10000        // ,
#define semicolon   0x20000        // ;
#define period      0x40000        // .
#define becomes     0x80000        // :=
#define beginsym    0x100000       // 保留字：begin
#define endsym      0x200000       // 保留字：end
#define ifsym       0x400000       // 保留字：if
#define thensym     0x800000       // 保留字：then
#define whilesym    0x1000000      // 保留字：while
#define dosym       0x2000000      // 保留字：do
#define callsym     0x4000000      // 保留字：call
#define constsym    0x8000000      // 保留字：const
#define varsym      0x10000000     // 保留字：var
#define procsym     0x20000000     // 保留字：procedure

enum object                        // 三种标识符的类型
{
    constant, variable, proc
};

enum fct                           // 中间代码指令集
{
    lit, opr, lod, sto, cal, Int, jmp, jpc
};

/*  lit 0, a : load constant a
    opr 0, a : execute operation a
    lod l, a : load variable l, a
    sto l, a : store variable l, a
    cal l, a : call procedure a at level l
    Int 0, a : increment t-register by a
    jmp 0, a : jump to a
    jpc 0, a : jump conditional to a       */

typedef struct                     // 指令类型
{
    enum fct f;	                   // 功能
    long l;                        // 层次差
    long a;                        // 数值/地址
} instruction;

char* err_msg[] =                  // 错误信息
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "",
/* 26 */    "",
/* 27 */    "",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "The number is too great.",
/* 32 */    "There are too many levels."
};


char ch;                           // 用于词法分析，存放最近一次从文件中读出的字符
unsigned long sym;                 // 存放最近一次识别出来的token类型
char id[al+1];                     // 存放最近一次识别出来的标识符的名字
long num;                          // 存放最近一次识别出来的数字
long cc;                           // 字母计数（列指针）
long ll;                           // 行长度
long kk, err;
long cx;                           // 代码分配指针，代码生成模块总在cx所指的位置生成新代码

char line[81];                     // 行缓冲区
char a[al+1];                      // 词法分析器中用于存放正在分析的词
instruction code[cxmax+1];         // 指令表，存放生成的中间代码
char word[norw][al+1];             // 保留字表
unsigned long wsym[norw];          // 保留字中每一个保留字对应的symbol类型表
unsigned long ssym[256];           // 符号对应的symbol类型表

char mnemonic[8][3+1];             // 中间代码助记符表
unsigned long declbegsys, statbegsys, facbegsys; //声明开始、表达式开始、factor开始符号集合

struct                             // 符号表
{
    char name[al+1];               // 符号名
    enum object kind;              // 符号类型
    long val;                      // 值
    long level;                    // 层差/偏移地址
    long addr;                     // 地址
}table[txmax+1];

char infilename[80];
FILE* infile;

// the following variables for block
long dx;                           // 数据分配指针
long lev;                          // 当前的块深度
long tx;                           // 当前的符号表指针

// the following array space for interpreter
#define stacksize 50000
long s[stacksize];                 // 数据栈
