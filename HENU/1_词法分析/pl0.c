// pl/0 compiler with code generation
#include <stdlib.h>
#include <string.h>
#include "pl0.h"

void error(long n)                 // 打印错误信息
{
    long i;

    printf("Error=>");
    for (i = 1; i <= cc-1; i++)
    {
        printf(" ");
    }

    printf("|%s(%d)\n", err_msg[n], n);

    err++;
}

void getch()                       // 读取下一个字符
{
    if(cc == ll)                   // 判断是否读完一行
    {
        if(feof(infile))           // 文件结束
        {
            printf("************************************\n");
            printf("      program incomplete\n");
            printf("************************************\n");
            exit(1);
        }

        ll = 0; cc = 0;

        printf("%5d ", cx);

        while((!feof(infile)) && ((ch=getc(infile))!='\n'))
        {
            printf("%c", ch);
            ll = ll + 1;
            line[ll] = ch;
        }

        printf("\n");

        ll = ll + 1;
        line[ll] = ' ';
    }

    cc = cc + 1;
    ch = line[cc];
}

//---------------------------------
//-------------词法分析
//---------------------------------
//从源文件中读出若干有效字符，组成一个 token 串，识别它的类型为保留字/标识
//符/数字或是其它符号。如果是保留字，把 sym 置成相应的保留字类型，如果是标
//识符，把 sym 置成 ident 表示是标识符，于此同时，id 变量中存放的即为保留
//字字符串或标识符名字。如果是数字，把 sym 置为 number,同时 num 变量中存
//放该数字的值。如果是其它的操作符，则直接把 sym 置成相应类型。经过本过程后
//ch 变量中存放的是下一个即将被识别的字符

void getsym()
{
    long i, j, k;

    while(ch == ' ' || ch == '\t') // 删除前导空格
    {
        getch();
    }

    if(isalpha(ch))                // 当前token的第一个字符为字母，可能是标识符或保留字
    {
        k = 0;                     // 用于记录token长度

        do                         // 读完一个token
        {
            if(k < al)             // 保证token长度不超过标识符最大长度
            {
                a[k] = ch; k = k + 1;
            }

            getch();
        } while(isalpha(ch) || isdigit(ch));

        if(k >= kk)                // 保证a数组的末尾都是空格
        {
            kk = k;
        }
        else
        {
            do
            {
                kk = kk-1; a[kk] = ' ';
            } while(k < kk);
        }

        strcpy(id, a); i = 0; j = norw - 1; //存放当前token的名字

        do                         // 在保留字表中二分查找该串是否存在
        {
            k = (i+j)/2;

            if(strcmp(id, word[k]) <= 0)
            {
                j = k - 1;
            }

            if(strcmp(id, word[k]) >=0)
            {
                i = k + 1;
            }
        } while(i <= j);

        if(i-1 > j)                // 找到则标记保留字
        {
            sym = wsym[k];
        }
        else                       // 否则标记为标识符
        {
            sym = ident;
        }
    }
    else if(isdigit(ch))           // 首字符为数字，则将其解析为数值
    {
        k = 0; num = 0; sym = number;
        do                         // 不断读取、保存数字
        {
            num = num * 10 + (ch - '0');
            k = k + 1;
            getch();
        } while(isdigit(ch));

        if(k > nmax)               // 位数超限
        {
            error(31);
        }
    }
    else if(ch == ':')             // 首字符为“:”，则可能是赋值
    {
        getch();

        if(ch == '=')
        {
            sym = becomes; getch();
        }
        else
        {
            sym = nul;
        }
    }
    else if(ch == '<')             // 首字符为“<”
    {
        getch();

        if(ch == '=')              // <=
        {
            sym = leq; getch();
        }
        else if(ch == '>')         // <>
        {
            sym=neq; getch();
        }
        else                       // <
        {
            sym = lss;
        }
    }
    else if(ch == '>')             // 首字符为“>”
    {
        getch();

        if(ch == '=')              // >=
        {
            sym=geq; getch();
        }
        else                       // >
        {
            sym=gtr;
        }
    }
    else                           // 均不是上述情况，则查询符号表，赋值
    {
        sym = ssym[(unsigned char)ch]; getch();
    }
}

//---------------------------------
//-------------中间代码生成
//---------------------------------
//将生成的中间代码写入中间代码数组，供后面的解释器运行

void gen(enum fct x, long y, long z)
{
    if(cx > cxmax)                 // 代码长度超限
    {
        printf("program too long\n");
        exit(1);
    }

    code[cx].f = x; code[cx].l = y; code[cx].a = z;

    cx = cx + 1;
}

//---------------------------------
//-------------测试当前单词是否合法
//---------------------------------
// 参数：s1:当语法分析进入或退出某一语法单元时当前单词符合应属于的集合
//      s2:在某一出错状态下，可恢复语法分析正常工作的补充单词集合
//      n :出错信息编号，当当前符号不属于合法的 s1 集合时发出的出错信息

void test(unsigned long s1, unsigned long s2, long n)
{
    if (!(sym & s1))               // 当前符号不在s1中
    {
        error(n);
        s1 = s1 | s2;              // 把s2补充进s1

        while(!(sym & s1))         // 循环找到下一个合法的符号
        {
            getsym();              // 继续词法分析
        }
    }
}

//---------------------------------
//-------------符号表
//---------------------------------

void enter(enum object k)
{
    tx = tx + 1;

    strcpy(table[tx].name, id);    // 保存符号名字

    table[tx].kind = k;            // 保存符号类型

    switch(k)
    {
        case constant:             // 常量
            if(num > amax)
            {
                error(31);
                num = 0;
            }
            table[tx].val = num;
            break;

        case variable:             // 变量
            table[tx].level = lev; // 记录当前层次
            table[tx].addr = dx;   // 记录层次中的偏移地址
            dx = dx + 1;           // 只有变量登录时需要用dx在数据栈中预留空间
            break;

        case proc:                 // 过程
            table[tx].level = lev;
            break;
    }
}

//---------------------------------
//-------------查找符号在符号表中的位置
//---------------------------------

long position(char* id)
{
    long i;

    strcpy(table[0].name, id);

    i=tx;

    while(strcmp(table[i].name, id) != 0)
    {
        i = i - 1;
    }

    return i;
}

//---------------------------------
//-------------常量声明
//---------------------------------

void constdeclaration()
{
    if(sym == ident)
    {
        getsym();

        if(sym == eql || sym == becomes) // 出现等号或赋值号
        {
            if(sym == becomes)     // 赋值号报错
            {
                error(1);
            }

            getsym();

            if(sym == number)      // 将数字登录到符号表
            {
                enter(constant);
                getsym();
            }
            else
            {
                error(2);
            }
        }
        else
        {
            error(3);
        }
    }
    else
    {
        error(4);
    }
}

//---------------------------------
//-------------变量声明
//---------------------------------

void vardeclaration()
{
    if(sym == ident)
    {
        enter(variable);           // 将标识符登录到符号表中
        getsym();
    }
    else
    {
        error(4);
    }
}

//---------------------------------
//-------------输出当前代码块的中间代码
//---------------------------------

void listcode(long cx0)
{
    long i;

    for(i=cx0; i<=cx-1; i++)
    {
        printf("%10d%5s%3d%5d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
    }
}

void expression(unsigned long);

//---------------------------------
//-------------factor处理
//---------------------------------
//fsys: 如果出错可用来恢复语法分析的符号集合

void factor(unsigned long fsys)
{
    long i;

    test(facbegsys, fsys, 24);     // 开始因子处理前，先检查当前 token 是否在 facbegsys 集合中
                                   // 如果不是合法的 token，抛 24 号错误，并通过 fsys 集恢复使语法处理可以继续进行

    while(sym & facbegsys)
    {
        if(sym == ident)           // 遇到标识符
        {
            i = position(id);      // 查符号表

            if(i==0)               // 标识符未定义
            {
                error(11);
            }
            else
            {
                switch(table[i].kind)
                {
                    case constant: //常量
                        gen(lit, 0, table[i].val);
                        break;

                    case variable: //变量
                        gen(lod, lev-table[i].level, table[i].addr);
                        break;

                    case proc:     //过程
                        error(21);
                        break;
                }
            }

            getsym();
        }
        else if(sym == number)     // 遇到数字
        {
            if(num>amax)
            {
                error(31); num=0;
            }

            gen(lit,0,num);
            getsym();
        }
        else if(sym == lparen)     // 遇到左括号
        {
            getsym();
            expression(rparen|fsys); // 后面是一个exp

            if(sym==rparen)        // 子表达式结束，应该遇到右括号
            {
                getsym();
            }
            else
            {
                error(22);
            }
        }

        test(fsys,lparen,23);      // 一个因子处理完毕，遇到的 token 应在 fsys 集合中
                                   // 如果不是，抛 23 号错，并找到下一个因子的开始，使语法分析可以继续运行下去
    }
}

//---------------------------------
//-------------term处理
//---------------------------------

void term(unsigned long fsys)
{
    unsigned long mulop;

    factor(fsys|times|slash);      // 每个term都应该从factor开始

    while(sym==times || sym==slash)// 处理乘除
    {
        mulop = sym;               // 保存当前运算符
        getsym();

        factor(fsys|times|slash);  // 运算符之后是一个factor

        if(mulop == times)
        {
            gen(opr,0,4);          // 乘法
        }
        else{
            gen(opr,0,5);          // 除法
        }
    }
}

//---------------------------------
//-------------exp处理
//---------------------------------

void expression(unsigned long fsys)
{
    unsigned long addop;

    if(sym==plus || sym==minus)    // 处理正负号
    {
        addop=sym;                 // 保存正负号
        getsym();

        term(fsys|plus|minus);     // 正负号后面是一个term

        if(addop==minus)
        {
            gen(opr,0,1);          // 负号，取反运算
        }
    }
    else
    {
        term(fsys|plus|minus);
    }

    while(sym==plus || sym==minus) // 处理加减
    {
        addop=sym;                 // 保存运算符
        getsym();

        term(fsys|plus|minus);     // 运算符后是一个term

        if(addop==plus)
        {
            gen(opr,0,2);          // 加
        }
        else
        {
            gen(opr,0,3);          // 减
        }
    }
}

//---------------------------------
//-------------cond处理
//---------------------------------

void condition(unsigned long fsys)
{
    unsigned long relop;

    if(sym==oddsym)                // odd运算符（一元）
    {
        getsym();
        expression(fsys);          // 后面是一个exp
        gen(opr, 0, 6);
    }
    else                           // 否则是个二元运算符
    {
        expression(fsys|eql|neq|lss|gtr|leq|geq); // 后面是一个exp

        if(!(sym&(eql|neq|lss|gtr|leq|geq)))
        {
            error(20);
        }
        else
        {
            relop=sym;             // 保存当前运算符
            getsym();

            expression(fsys);      // 处理表达式右边

            switch(relop)
            {
                case eql:
                    gen(opr, 0, 8);
                    break;

                case neq:
                    gen(opr, 0, 9);
                    break;

                case lss:
                    gen(opr, 0, 10);
                    break;

                case geq:
                    gen(opr, 0, 11);
                    break;

                case gtr:
                    gen(opr, 0, 12);
                    break;

                case leq:
                    gen(opr, 0, 13);
                    break;
            }
        }
    }
}

//---------------------------------
//-------------stmt处理
//---------------------------------

void statement(unsigned long fsys)
{
    long i,cx1,cx2;

    if(sym==ident)                 // 标识符
    {
        i=position(id);            // 查符号表
        if(i==0)
        {
            error(11);             // 未定义
        }
        else if(table[i].kind!=variable)
        {
            error(12);             // 非变量
            i=0;
        }

        getsym();

        if(sym==becomes)           // 赋值
        {
            getsym();
        }
        else
        {
            error(13);
        }

        expression(fsys);          // 后面应该是一个exp

        if(i!=0)                   // 若未出错，则产生一个sto代码
        {
            gen(sto,lev-table[i].level,table[i].addr);
        }
    }
    else if(sym==callsym)          // call语句
    {
        getsym();
        if(sym!=ident)
        {
            error(14);
        }
        else
        {
            i=position(id);
            if(i==0)
            {
                error(11);
            }
            else if(table[i].kind==proc)
            {
                gen(cal,lev-table[i].level,table[i].addr);
            }
            else
            {
                error(15);
            }

            getsym();
        }
    }
    else if(sym==ifsym)            // if语句
    {
        getsym();
        condition(fsys|thensym|dosym);// 后面是一个cond

        if(sym==thensym)
        {
            getsym();
        }
        else
        {
            error(16);
        }
        cx1=cx;
        gen(jpc,0,0);
        statement(fsys);           // 后面是一个stmt
        code[cx1].a=cx;
    }
    else if(sym==beginsym)         // begin语句
    {
        getsym();
        statement(fsys|semicolon|endsym); // 后面是stmt
        while(sym==semicolon||(sym&statbegsys)) // 处理分号和语句
        {
            if(sym==semicolon)     // 分号
            {
                getsym();
            }
            else
            {
                error(10);
            }
            statement(fsys|semicolon|endsym);
        }
        if(sym==endsym)
        {
            getsym();
        }
        else
        {
            error(17);
        }
    }
    else if(sym==whilesym)         // while语句
    {
        cx1=cx;                    // 记录中间代码起始指针
        getsym();
        condition(fsys|dosym);     // 后面是一个cond
        cx2=cx;                    // 记录中间代码位置，要放退出地址
        gen(jpc,0,0);
        if(sym==dosym)             // do语句
        {
            getsym();
        }
        else
        {
            error(18);
        }

        statement(fsys);           // 后面是stmt
        gen(jmp,0,cx1);            // 循环跳转

        code[cx2].a=cx;            // 把退出地址补上
    }

    test(fsys,0,19);
}

//---------------------------------
//-------------block处理
//---------------------------------

void block(unsigned long fsys)
{
    long tx0;
    long cx0;
    long tx1;
    long dx1;

    dx=3;
    // 地址寄存器给出每层局部量当前已分配到的相对位置
    // 置初始值为 3 的原因是：每一层最开始的位置有三个空间用于存放
    // 静态链 SL、动态链 DL 和 返回地址 RA
    tx0=tx;                        // 记录本层开始时符号表的位置
    table[tx].addr=cx;             // 符号表记下当前层代码的开始地址
    gen(jmp,0,0);                  // block开始时首先写下一句跳转指令，地址到后面再补

    if(lev>levmax)                 // 嵌套层数太大
    {
        error(32);
    }

    do                             // 开始处理声明部分
    {
        if(sym==constsym)          // 常量
        {
            getsym();

            do
            {
                constdeclaration();// 常量声明
                while(sym==comma)  // 逗号
                {
                    getsym();
                    constdeclaration();// 逗号后面继续常量声明
                }

                if(sym==semicolon) // 分号
                {
                    getsym();
                }
                else
                {
                    error(5);
                }
            } while(sym==ident);
        }

        if(sym==varsym)            // 变量
        {
            getsym();
            do
            {
                vardeclaration();  // 变量声明
                while(sym==comma)  // 逗号
                {
                    getsym();
                    vardeclaration();// 逗号后面继续声明变量
                }

                if(sym==semicolon) // 分号
                {
                    getsym();
                }
                else
                {
                    error(5);
                }
            } while(sym==ident);
        }

        while(sym==procsym)        // 过程
        {
            getsym();
            if(sym==ident)         // 标识符
            {
                enter(proc);       // 将过程登记进符号表
                getsym();
            }
            else
            {
                error(4);
            }

            if(sym==semicolon)     // 分号
            {
                getsym();
            }
            else
            {
                error(5);
            }

            lev=lev+1;             // 层级加1
            tx1=tx;                // 记录当前层级
            dx1=dx;                // 记录当前数据指针
            block(fsys|semicolon); // 处理block
            lev=lev-1;
            tx=tx1;
            dx=dx1;                // 恢复

            if(sym==semicolon)     // 分号
            {
                getsym();
                test(statbegsys|ident|procsym,fsys,6);
                // 检查当前 token 是否合法，不合法 则用 fsys 恢复语法分析同时抛 6 号错
            }
            else
            {
                error(5);
            }
        }

        test(statbegsys|ident,declbegsys,7);
        // 检查当前状态是否合法，不合法则用声明开 始符号作出错恢复、抛 7 号错
    } while(sym&declbegsys);

    code[table[tx0].addr].a=cx;    // 把block开头写下的跳转指令的地址补上
    table[tx0].addr=cx;            // tx0的符号表存的是当前block的参数
    cx0=cx;
    gen(Int,0,dx);
    statement(fsys|semicolon|endsym);
    gen(opr,0,0); // return
    test(fsys,0,8);
    //listcode(cx0);
}

//---------------------------------
//-------------通过静态链求出数据区基地址
//---------------------------------

long base(long b, long l)
{
    long b1;

    b1=b;

    while (l>0)                    // 根据层级差来找到基地址
    {
        b1=s[b1]; l=l-1;
    }

    return b1;
}

//---------------------------------
//-------------指令集解释器
//---------------------------------

void interpret()
{
    long p,b,t;		// 程序寄存器PC、基地址寄存器、栈顶寄存器
    instruction i;	// 指令寄存器

    printf("start PL/0\n");
    t=0; b=1; p=0;
    s[1]=0; s[2]=0; s[3]=0;

    do
    {
        i=code[p]; p=p+1;          // 每次在code表中读取一条指令

        switch(i.f)
        {
            case lit:              // 常数指令
                t=t+1; s[t]=i.a;
                break;

            case opr:              // 运算指令
                switch(i.a)
                {
                    case 0:        // 返回指令
                        t=b-1; p=s[t+3]; b=s[t+2];
                        break;

                    case 1:        // 负号
                        s[t]=-s[t];
                        break;

                    case 2:        // 加法
                        t=t-1; s[t]=s[t]+s[t+1];
                        break;

                    case 3:        // 减法
                        t=t-1; s[t]=s[t]-s[t+1];
                        break;

                    case 4:        // 乘法
                        t=t-1; s[t]=s[t]*s[t+1];
                        break;

                    case 5:        // 除法
                        t=t-1; s[t]=s[t]/s[t+1];
                        break;

                    case 6:        // odd
                        s[t]=s[t]%2;
                        break;

                    case 8:        // ==
                        t=t-1; s[t]=(s[t]==s[t+1]);
                        break;

                    case 9:        // !=
                        t=t-1; s[t]=(s[t]!=s[t+1]);
                        break;

                    case 10:       // <
                        t=t-1; s[t]=(s[t]<s[t+1]);
                        break;

                    case 11:       // >=
                        t=t-1; s[t]=(s[t]>=s[t+1]);
                        break;

                    case 12:       // >
                        t=t-1; s[t]=(s[t]>s[t+1]);
                        break;

                    case 13:       // <=
                        t=t-1; s[t]=(s[t]<=s[t+1]);
                }
                break;

            case lod:              // 调用变量值指令
                t=t+1; s[t]=s[base(b,i.l)+i.a];
                break;

            case sto:              // 将值存入变量指令
                s[base(b,i.l)+i.a]=s[t]; printf("%10d\n", s[t]); t=t-1;
                break;

            case cal:              // 过程调用，产生新的块标记
                s[t+1]=base(b,i.l);
                s[t+2]=b;
                s[t+3]=p;          // 记录返回地址等参数
                b=t+1; p=i.a;
                break;

            case Int:              // 开内存空间
                t=t+i.a;
                break;

            case jmp:              // 无条件跳转指令
                p=i.a;
                break;

            case jpc:              // 栈顶为0跳转
                if(s[t]==0)
                {
                    p=i.a;
                }
                t=t-1;
        }
    } while(p!=0);
    printf("end PL/0\n");
}

//---------------------------------
//-------------PL/0编译器主函数
//---------------------------------

int main()
{
    long i;

    //-------------初始化

    for(i=0; i<256; i++)
    {
        ssym[i]=nul;
    }

    strcpy(word[0],  "begin     ");// 保留字
    strcpy(word[1],  "call      ");
    strcpy(word[2],  "const     ");
    strcpy(word[3],  "do        ");
    strcpy(word[4],  "end       ");
    strcpy(word[5],  "if        ");
    strcpy(word[6],  "odd       ");
    strcpy(word[7],  "procedure ");
    strcpy(word[8],  "then      ");
    strcpy(word[9],  "var       ");
    strcpy(word[10], "while     ");

    wsym[0]=beginsym;              // 保留字对应的symbol类型表
    wsym[1]=callsym;
    wsym[2]=constsym;
    wsym[3]=dosym;
    wsym[4]=endsym;
    wsym[5]=ifsym;
    wsym[6]=oddsym;
    wsym[7]=procsym;
    wsym[8]=thensym;
    wsym[9]=varsym;
    wsym[10]=whilesym;

    ssym['+']=plus;                // 符号对应的symbol类型表
    ssym['-']=minus;
    ssym['*']=times;
    ssym['/']=slash;
    ssym['(']=lparen;
    ssym[')']=rparen;
    ssym['=']=eql;
    ssym[',']=comma;
    ssym['.']=period;
    ssym[';']=semicolon;

    strcpy(mnemonic[lit],"LIT");   // 中间代码助记符表
    strcpy(mnemonic[opr],"OPR");
    strcpy(mnemonic[lod],"LOD");
    strcpy(mnemonic[sto],"STO");
    strcpy(mnemonic[cal],"CAL");
    strcpy(mnemonic[Int],"INT");
    strcpy(mnemonic[jmp],"JMP");
    strcpy(mnemonic[jpc],"JPC");

    declbegsys=constsym|varsym|procsym;// 声明开始符号集合
    statbegsys=beginsym|callsym|ifsym|whilesym;// 表达式开始符号集合
    facbegsys=ident|number|lparen; // factor开始符号集合

    printf("please input source program file name: ");
    scanf("%s",infilename);
    printf("\n");

    if((infile=fopen(infilename,"r"))==NULL)
    {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }


    //-------------开始语法分析

    err=0;
    cc=0;
    cx=0;
    ll=0;                          // 各个计数器初始化
    ch=' ';                        // 为第一次读取字符作初始化
    kk=al;
    getsym();
    lev=0; tx=0;
    block(declbegsys|statbegsys|period);

    if(sym!=period)                // 点
    {
        error(9);
    }

    //-------------语法分析完成

    if(err==0)                     // 程序无错误则开始解释执行
    {
       // interpret();
       listcode(0);
    }
    else
    {
        printf("errors in PL/0 program\n");
    }

    fclose(infile);

    return (0);
}
