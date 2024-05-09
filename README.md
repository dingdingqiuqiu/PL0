### 目录结构
````
.
├── Code
│   ├── DIY
│   │   ├── main.c
│   │   ├── pl0.i
│   │   ├── pl0.pas
│   │   └── scanner
│   └── LEX
│       ├── lex.yy.c
│       ├── main.c
│       ├── pl0.l.l
│       ├── pl0.pas
│       └── scanner
└── Doc
    ├── 编译原理_实验指导书.doc
    ├── 词法分析-实验步骤.md
    └── 学号-姓名-编译原理-实验一.docx


````
### 手写词法分析器
`./Code/DIY/main.c`为手写编译器词法分析源代码
在`./Code/DIY`目录下执行下面代码得到目标程序
```
gcc main.c -o scanner
```
运行得到词法分析结果
```
./scanner pl0.pas
```
### LEX生成词法分析器
`./Code/LEX/pl0.l.l`为输入`flex`源文件
在`./Code/LEX`目录下执行下面代码得到目标程序源码
```
flex pl0.l.l
```
编译目标程序的c源代码
```
gcc lex.yy.c -lfl -o scanner                      
```
运行目标程序得到词法分析结果
```
./scanner pl0.pas
```
