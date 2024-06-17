# MakeFile文件格式
# 目标(Target)： 依赖(Dependencies)
# 命令(Commands)

# 定义编译器
CC=g++

# 定义编译标志-w忽略警告信息
CCFLAGS=-w

# 定义目标文件
TARGET1=cifa
TARGET2=yufa
TARGET3=yuyi

# 定义源文件
SRC1=main-cifa.c
SRC2=main-yufa.c
SRC3=main-yuyi.c

# 定义输入分析器文件
INPUT=pl0.pas

# 定义生成的中间文件
TMP=pl0.i

# 默认编译三个文件
all: $(TARGET1) $(TARGET2) $(TARGET3)

# 编译词法分析器
$(TARGET1): $(SRC1)
	$(CC) $(CCFLAGS) $(SRC1) -o $(TARGET1)

# 编译语法分析器
$(TARGET2): $(SRC2)
	$(CC) $(CCFLAGS) $(SRC2) -o $(TARGET2)

# 编译语义分析器
$(TARGET3): $(SRC3)
	$(CC) $(CCFLAGS) $(SRC3) -o $(TARGET3)

# 清理可执行文件
clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TMP)

# 测试各个分析器功能
test: test1 test2 test3

# 测试词法分析器功能
test1:
	./$(TARGET1) $(INPUT)

# 测试语法分析器功能
test2:
	./$(TARGET2) $(INPUT)

# 测试语义分析器功能
test3:
	./$(TARGET3) $(INPUT)

# .PHONY用来声明为伪目标
# 即执行make all或make clean或make test1或make test2或make test3或make test时
# make程序直接执行命令而非寻找all文件或clean文件或test1文件或test2文件或test3文件
# 或test文件
.PHONY: all clean test1 test2 test3 test
