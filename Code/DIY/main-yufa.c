#include <unordered_map>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cassert>
#include <map>
#include <algorithm>
#include <fstream>
using namespace std;

#ifdef Nanfeng1997
#include <debugger>
#endif

//词法分析器
//以下是我对单词种别划分
/*
  关键字：
  begin       1
  end         2
  if          3
  then        4
  while       5
  do          6
  const       7
  var         8
  call        9
  procedure   10
  odd         11

  运算符：
  +           12
  -           13
  *           14
  /           15
  =           16
  #           17
  <           18
  >           19
  =           20
  (           21
  )           22
  ,           23
  .           24
  ;           25

  标识符：    26
  常数：      27
*/

vector<string> key, identify, symbol;
unordered_map<string, int> key_map, identify_map, symbol_map; // value
unordered_map<string, int> letter_map; // Type

int key_offset = 1;
int symbol_offset = 12;

struct node {
  int type;
  string val;
};  

const int numbers_type = 27;
const int identify_type = 26;
vector<node> tokens; //词法分析结果
int index = 0; //tokens的下标

void init() {
  string str_key = "begin  end  if  then  while  do  const  var  call  procedure  odd", s;
  string str_symbol = "+  -  *  /  =  #  <  >  :=  (  )  ,  .  ;";
  stringstream key_in;
  key_in << str_key;
  while (key_in >> s) {
    key.push_back(s);
    key_map[s] = int(key.size()) - 1;
    letter_map[s] = int(key.size()) - 1 + key_offset;
  }
  stringstream symbolin;
  symbolin << str_symbol;
  while (symbolin >> s) {
    symbol.push_back(s);
    symbol_map[s] = int(symbol.size()) - 1;
    letter_map[s] = int(symbol.size()) - 1 + symbol_offset;
  }

}

void read(vector<string> &ans) {
  if (not ans.empty()) ans.clear();
  string str, res, s; 
  while (getline(cin, str)) {
    if (str.find("//") != string::npos) {
      str.erase(str.find("//"));
    }
    res += str; res.push_back('\n');
  }
  
  stringstream ss; ss << res;
  int flag = 1;
  while (ss >> s) {
    if (s == "(*") flag ^= 1;
    
    if (flag) {
      ans.push_back(s);
    }

    if (s == "*)") flag ^= 1;
  }
}
// （种别，属性值）
void LexicalAnalysis(vector<string> &str, vector<node> &ans) {
  if (not ans.empty()) ans.clear();
  for (string s: str) {
    // cout << s << " ";
    if (key_map.count(s)) { //
      ans.push_back({letter_map[s], "-"});
    } else if (symbol_map.count(s)) {
      if (int(s.size()) > 10) {
        cout << "symbol's size is too long!\n";
        assert(false);
      }
      ans.push_back({letter_map[s], "-"});
    } else if (identify_map.count(s)) {
      ans.push_back({identify_type, to_string(identify_map[s])});
    } else {
      // continue;
      int ns = s.size();
      for (int i = 0; i < ns; i ++ ) {
        if (isdigit(s[i])) {
          string t; t += s[i];
          int j = i; while (j + 1 < ns and isdigit(s[j + 1])) {
            ++ j;
            t += s[j];
          }
          ans.push_back({numbers_type, t});    
          i = j;
        } else if (isalpha(s[i])) {
          string t; t += s[i];
          int j = i; while (j + 1 < ns and (isalpha(s[j + 1]) or isdigit(s[j + 1]))) {
            ++ j;
            t += s[j];
          }
          if (key_map.count(t)) {
            ans.push_back({letter_map[t], "-"});    
          } else {
            if (int(t.size()) > 10) {
              cout << "identify's size is too long!\n";
              assert(false);
            }
            if (!identify_map.count(t)) {
              identify.push_back(t);
              int new_val = (int)identify.size() - 1;
              identify_map[t] = new_val;
            }
            ans.push_back({identify_type, to_string(identify_map[t])});
          }
          i = j;
        } else {
          string t; t += s[i];
          if (symbol_map.count(t)) {
            ans.push_back({letter_map[t], "-"});
          } else {
            if (i + 1 >= (int)s.size()) {
              cout << "illegal!\n";
              assert(false);
            }
            t += s[++ i];
            if (symbol_map.count(t)) {
              ans.push_back({letter_map[t], "-"});
            } else {
              assert(false);
            }
          }
        }
      }
    }
  }
}

bool isDigit(string s) {
  if (s[0] == '-') {
    reverse(s.begin(), s.end()); 
    s.pop_back();
    reverse(s.begin(), s.end());
    if (s == "0") return false;
  }
  if (s.empty()) return false;
  if (s[0] == '0') {
    return int(s.size()) == 1;
  } else {
    for (auto c: s) if (c < '0' or c > '9') return false;
  }
  return true;
}

//对实验文档中的消除左递归和回溯的PL/0的EBNF文法分析
/*
    program：程序，由 block 和 "." 组成。
    block：代码块，包含 constdecl、vardecl、procdecl 和 statement 四部分。
    constdecl：常量声明，由可选的 "const" 关键字，多个 constitem，以及 ";" 组成。
    constitem：常量条目，包括标识符 ident 和数字 number 之间的 "=" 号连接。
    vardecl：变量声明，由可选的 "var" 关键字，多个标识符 ident，以及 ";" 组成。
    procdecl：过程声明，由多个 "procedure" 关键字，标识符 ident，代码块 block 和 ";" 组成。
    statement：语句，包括赋值语句 assignstmt、过程调用语句 callstmt、复合语句 compstmt、条件语句 ifstmt 和循环语句 whilestmt。
    assignstmt：赋值语句，包括标识符 ident，":=" 连接后面的 expression。
    callstmt：过程调用语句，包括 "call" 关键字和标识符 ident。
    compstmt：复合语句，由 "begin" 关键字和多个语句 statement，以及 "end" 关键字组成。
    ifstmt：条件语句，由 "if" 关键字、condition、"then" 关键字和语句 statement 组成。
    whilestmt：循环语句，由 "while" 关键字、condition、"do" 关键字和语句 statement 组成。
    condition：条件判断，包括 "odd" 和 expression 之间的关系运算符（"="、"#"、"<"、">"）。
    expression：表达式，由 term 和多个加减法运算符组成。
    term：项，由 factor 和多个乘除法运算符组成。
    factor：因子，包括可选的正负号、标识符 ident、数字 number 和括号中的 expression。
*/

//函数列表，注释为对应语法单位
string get_str(node &nd); //通过 node 获取读入的字符串
void exception_print(int type, string expected);    //打印异常
void match(string expected);           //单输入匹配函数
void match(string expected, bool majority);             //多输入匹配函数  E -> aB | B
void parse_program();   //program -> block "."
void parse_block();     //block -> constdecl vardecl procdecl statement
void parse_constdecl(); //constdecl ->["const" constitem{ "," constitem } ";"]
void parse_constitem(); //constitem -> ident "=" number
void parse_vardecl();   //vardecl -> ["var" ident {"," ident} ";"]
void parse_procdecl();  //procdecl -> {"procedure" ident ";" block ";"}
void parse_statement(); //statement -> assignstmt | callstmt | compstmt | ifstmt | whilestmt
void parse_assignstmt();//assignstmt ->[ident ":=" expression]
void parse_callstmt();  //callstmt -> ["call" ident]
void parse_compstmt();  //compstmt -> ["begin" statement {";" statement} "end"]
void parse_ifstmt();    //ifstmt -> ["if" condition "then" statement]
void parse_whilestmt(); //whilestmt -> ["while" condition "do" statement]
void parse_condition(); //condition = "odd" expression | expression ("="|"#"|"<"|">") expression
void parse_expression();//expression -> term { ("+"|"-") term}
void parse_term();      //term -> factor {("*"|"/") factor}
void parse_factor();    //factor -> [ "+"|"-"] (ident | number | "(" expression ")")
void parse_ident();     //ident -> letter {letter | digit}
void parse_number();    //number -> digit {digit} 


bool success = true;
string symbolarray[] = { "=", "#", "<", ">" };
int symbollength = 4;


string get_str (node &nd) {
  auto [type, val] = nd;
  if (type >= 1 and type <= 11) {
    return key[type - key_offset];
  } else if (type >= 12 and type <= 25) {
    return symbol[type - symbol_offset];
  } else if (type == 26) {
    return identify[stoi(val)];
  } else if (type == 27) {
    return val;
  } else {
    assert(false);
  }
  return "error";
}

void exception_print(int type, string expected = "") {
  string ret;
  if (type == 0) { //语法错误
    ret = "语法错误：预期为 " + expected + "，但输入的是 " + get_str(tokens[index]);
  } else if (type == 1) {
    ret = "标识符 " + get_str(tokens[index]) + " 格式错误";
  } else if (type == 2) {
    ret = "整数 " + get_str(tokens[index]) + "格式错误";
  }
  cout << ret << "\n";
  success = false;
  exit(100);
}


void match(string expected) {
  if (get_str(tokens[index]) == expected) {
    index++;
  } else {
    exception_print(0, expected);
  }
}

void match(string expected, bool majority) {
  bool iseuqal = false;
  for (int i = 0; i < symbollength; i++) {
    if (get_str(tokens[index]) == symbolarray[i]) {
      index += 1; iseuqal = true; break;
    }
    expected += symbolarray[i];
    if (i != symbollength - 1) {
      expected += " ";
    }
  }
  if (!iseuqal) {
    exception_print(0, expected);
  }
}

void parse_program() {
  parse_block();
  match(".");
}

void parse_block() {
  parse_constdecl();
  parse_vardecl();
  parse_procdecl();
  parse_statement();
}

void parse_constdecl() {
  if (get_str(tokens[index]) == "const") {
    match("const");
    parse_constitem();
    while (get_str(tokens[index]) == ",") {
      match(",");
      parse_constitem();
    }
    match(";");
  }
}

void parse_constitem() {
  parse_ident();
  match("=");
  parse_number();
}

void parse_vardecl() {
  if (get_str(tokens[index]) == "var") {
    match("var");
    parse_ident();
    while (get_str(tokens[index]) == ",") {
      match(",");
      parse_ident();
    }
    match(";");
  }
}

void parse_procdecl() {
  while (get_str(tokens[index]) == "procedure") {
    match("procedure");
    parse_ident();
    match(";");
    parse_block();
    match(";");
  }
}

void parse_statement() {
  if (identify_map.count(get_str(tokens[index]))) {
    parse_assignstmt();
  } else if (get_str(tokens[index]) == "call") {
    parse_callstmt();
  } else if (get_str(tokens[index]) == "begin") {
    parse_compstmt();
  } else if (get_str(tokens[index]) == "if") {
    parse_ifstmt();
  } else if (get_str(tokens[index]) == "while") {
    parse_whilestmt();
  } 
}

void parse_assignstmt() {
  parse_ident();
  match(":=");
  parse_expression();
}

void parse_callstmt() {
  match("call");
  parse_ident();
}

void parse_compstmt() {
  match("begin");
  parse_statement();
  while (get_str(tokens[index]) == ";") {
    match(";");
    parse_statement();
  }
  match("end");
}

void parse_ifstmt() {
  match("if");
  parse_condition();
  match("then");
  parse_statement();
}

void parse_whilestmt() {
  match("while");
  parse_condition();
  match("do");
  parse_statement();
}

void parse_condition() {
  if (get_str(tokens[index]) == "odd") {
    match("odd");
    parse_expression();
  } else {
    parse_expression();
    match("", true);
    parse_expression();
  }
}

void parse_expression() {
  parse_term();
  while (get_str(tokens[index]) == "+" || get_str(tokens[index]) == "-") {
    match(get_str(tokens[index]));
    parse_term();
  }
}

void parse_term() {
  parse_factor();
  while (get_str(tokens[index]) == "*" || get_str(tokens[index]) == "/") {
    match(get_str(tokens[index]));
    parse_factor();
  }
}

void parse_factor() {
  if (get_str(tokens[index]) == "+" || get_str(tokens[index]) == "-") {
    match(get_str(tokens[index]));
  } 
  if (isDigit(get_str(tokens[index]))) {
    parse_number();
  } else if (identify_map.count(get_str(tokens[index]))) {
    parse_ident();
  } else {
    match("(");
    parse_expression();
    match(")");
  }
}

void parse_ident() {
  if (identify_map.count(get_str(tokens[index]))) {
    match(get_str(tokens[index]));
  } else {
    exception_print(1);
  }
}

void parse_number() {
  if (isDigit(get_str(tokens[index]))) {
    match(get_str(tokens[index]));
  } else {
    exception_print(2);
  }
}


bool GrammaticalAnalysis() {
  parse_program();
  if (success) {
    cout << "语法正确" << endl;
  }
  return success;
}

int main(int argc,char* argv[]) {
  if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "Failed to open file: " << argv[1] << endl;
        return 1;
    }

    // 重定向 cin 到文件输入
    cin.rdbuf(infile.rdbuf());




    init();
    vector<string> str;
    read(str);
  
    LexicalAnalysis(str, tokens);

    GrammaticalAnalysis();
    
    return 0;
}
