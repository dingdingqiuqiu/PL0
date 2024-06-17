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
int tempVarNumber = 1;

struct section {
    string op, arg1, arg2, result;
};

vector<section> Sec0, Sec;

struct node {
    int type;
    string val;
};

struct TreeNode {
    int type;
    string val;
    vector<TreeNode*> son;
    string tempVarName;
    TreeNode() {}
    TreeNode(node nd) { type = nd.type; val = nd.val; son = {}; tempVarName = ""; }
    TreeNode(node nd, string name) { type = nd.type; val = nd.val; son = {}; tempVarName = name; }
    int get_son() { return son.size(); }
    void add(TreeNode* Node) {
        son.push_back(Node);
    }
};

string getTempNumber() {
    string ans = "T" + to_string(tempVarNumber);
    tempVarNumber += 1;
    return ans;
}

// + - * / :=  > < >= <= 
TreeNode* opr(node op, int num, vector<TreeNode*> allNode) {
    TreeNode* rt = new TreeNode(op, "-");
    for (auto Item : allNode) {
        rt->add(Item);
    }
    return rt;
}

TreeNode* opr(node op, int num, vector<TreeNode*> allNode, string name) {
    TreeNode* rt = new TreeNode(op, name);
    for (auto Item : allNode) {
        rt->add(Item);
    }
    return rt;
}

TreeNode* id(node entry) {
    TreeNode* rt = new TreeNode(entry, "-");
    return rt;
}

TreeNode* id(node entry, string name) {
    TreeNode* rt = new TreeNode(entry, name);
    return rt;
}


TreeNode* num(node entry) {
    TreeNode* rt = new TreeNode(entry);
    return rt;
}

const int numbers_type = 27;
const int identify_type = 26;
vector<node> tokens; //词法分析结果
int index = 0; //tokens的下标
node nEndD;
TreeNode* root;

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

void read(vector<string>& ans) {
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
void LexicalAnalysis(vector<string>& str, vector<node>& ans) {
    cout << "Staring LexicalAnalysis..." << endl;
    if (not ans.empty()) ans.clear();
    for (string s : str) {
        // cout << s << " ";
        if (key_map.count(s)) { //
            ans.push_back({ letter_map[s], "-" });
        }
        else if (symbol_map.count(s)) {
            if (int(s.size()) > 10) {
                cout << "symbol's size is too long!\n";
                assert(false);
            }
            ans.push_back({ letter_map[s], "-" });
        }
        else if (identify_map.count(s)) {
            ans.push_back({ identify_type, to_string(identify_map[s]) });
        }
        else {
            // continue;
            int ns = s.size();
            for (int i = 0; i < ns; i++) {
                if (isdigit(s[i])) {
                    string t; t += s[i];
                    int j = i; while (j + 1 < ns and isdigit(s[j + 1])) {
                        ++j;
                        t += s[j];
                    }
                    ans.push_back({ numbers_type, t });
                    i = j;
                }
                else if (isalpha(s[i])) {
                    string t; t += s[i];
                    int j = i; while (j + 1 < ns and (isalpha(s[j + 1]) or isdigit(s[j + 1]))) {
                        ++j;
                        t += s[j];
                    }
                    if (key_map.count(t)) {
                        ans.push_back({ letter_map[t], "-" });
                    }
                    else {
                        if (int(t.size()) > 10) {
                            cout << "identify's size is too long!\n";
                            assert(false);
                        }
                        if (!identify_map.count(t)) {
                            identify.push_back(t);
                            int new_val = (int)identify.size() - 1;
                            identify_map[t] = new_val;
                        }
                        ans.push_back({ identify_type, to_string(identify_map[t]) });
                    }
                    i = j;
                }
                else {
                    string t; t += s[i];
                    if (symbol_map.count(t)) {
                        ans.push_back({ letter_map[t], "-" });
                    }
                    else {
                        if (i + 1 >= (int)s.size()) {
                            cout << "illegal!\n";
                            assert(false);
                        }
                        t += s[++i];
                        if (symbol_map.count(t)) {
                            ans.push_back({ letter_map[t], "-" });
                        }
                        else {
                            assert(false);
                        }
                    }
                }
            }
        }
    }
    nEndD = ans.back();
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
    }
    else {
        for (auto c : s) if (c < '0' or c > '9') return false;
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
string get_str(node nd); //通过 node 获取读入的字符串
void exception_print(int type, string expected);    //打印异常
void match(string expected);           //单输入匹配函数
void match(string expected, bool majority);             //多输入匹配函数  E -> aB | B
bool match(string expected, char flag);
TreeNode* parse_program();   //program -> block "."
vector<TreeNode*> parse_block();     //block -> constdecl vardecl procdecl statement
TreeNode* parse_constdecl(); //constdecl ->["const" constitem{ "," constitem } ";"]
TreeNode* parse_constitem(); //constitem -> ident "=" number
TreeNode* parse_vardecl();   //vardecl -> ["var" ident {"," ident} ";"]
TreeNode* parse_procdecl();  //procdecl -> {"procedure" ident ";" block ";"}
TreeNode* parse_statement(); //statement -> assignstmt | callstmt | compstmt | ifstmt | whilestmt
TreeNode* parse_assignstmt();//assignstmt ->[ident ":=" expression]
TreeNode* parse_callstmt();  //callstmt -> ["call" ident]
TreeNode* parse_compstmt();  //compstmt -> ["begin" statement {";" statement} "end"]
TreeNode* parse_ifstmt();    //ifstmt -> ["if" condition "then" statement]
TreeNode* parse_whilestmt(); //whilestmt -> ["while" condition "do" statement]
TreeNode* parse_condition(); //condition = "odd" expression | expression ("="|"#"|"<"|">") expression
TreeNode* parse_expression();//expression -> term { ("+"|"-") term}
TreeNode* parse_term();      //term -> factor {("*"|"/") factor}
TreeNode* parse_factor();    //factor -> [ "+"|"-"] (ident | number | "(" expression ")")
TreeNode* parse_ident();     //ident -> letter {letter | digit}
TreeNode* parse_number();    //number -> digit {digit} 
void print(section x);
void print0(section x);

bool success = true;
string symbolarray[] = { "=", "#", "<", ">" };
int symbollength = 4;


string get_str(node nd) {
    // auto [type, val] = nd;
    int type = nd.type;
    string val = nd.val;
    if (type >= 1 and type <= 11) {
        return key[type - key_offset];
    }
    else if (type >= 12 and type <= 25) {
        return symbol[type - symbol_offset];
    }
    else if (type == 26) {
        return identify[stoi(val)];
    }
    else if (type == 27) {
        return val;
    }
    else {
        assert(false);
    }
    return "error";
}

void print(TreeNode* Rt) {
    cout << get_str(node{ Rt->type, Rt->val }) << "   \n";
}


void exception_print(int type, string expected = "") {
    string ret;
    if (type == 0) { //语法错误
        ret = "语法错误：预期为 " + expected + "，但输入的是 " + get_str(tokens[index]);
    }
    else if (type == 1) {
        ret = "标识符 " + get_str(tokens[index]) + " 格式错误";
    }
    else if (type == 2) {
        ret = "整数 " + get_str(tokens[index]) + "格式错误";
    }
    cout << ret << "\n";
    success = false;
}


void match(string expected) {
    if (get_str(tokens[index]) == expected) {
        index++;
    }
    else {
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

bool match(string expected, char flag) {
    if (get_str(tokens[index]) == "#") {
        index += 1;
        return true;
    }
    else {
        return false;
    }
}

int limit, f1, f2, t, n;
bool haveLimit, haveAns;
map<int, int> mp;

int find(int x) {
    if (x <= 1) return x;
    if (mp.count(x)) return mp[x];
    mp[x] = find(x - 2) + find(x - 1);
    return mp[x];
}


TreeNode* parse_program() {
    auto tmpP = parse_block();
    node nD = tokens[index];
    match(".");
    auto ans = opr(nD, 4, tmpP);
    return ans;
}

vector<TreeNode*> parse_block() {
    return vector<TreeNode*>{parse_constdecl(), parse_vardecl(), parse_procdecl(), parse_statement()};
}

TreeNode* parse_constdecl() {
    TreeNode* tmpP = NULL;

    if (get_str(tokens[index]) == "const") {
        node nConst = tokens[index];
        match("const");
        tmpP = parse_constitem();

        while (get_str(tokens[index]) == ",") {
            node nD = tokens[index];
            match(",");
            tmpP = opr(nD, 2, vector<TreeNode*>{tmpP, parse_constitem()});
        }
        match(";");
        tmpP = opr(nConst, 1, vector<TreeNode*>{tmpP});
    }
    return tmpP;
}

TreeNode* parse_constitem() {
    TreeNode* tmpP = id(tokens[index]);
    parse_ident();
    auto Equal = tokens[index];
    match("=");
    Sec0.push_back(section{ "const", get_str(node{tmpP->type, tmpP->val}), get_str(tokens[index]), "-" });
    tmpP = opr(Equal, 2, vector<TreeNode*>{tmpP, num(tokens[index])});
    parse_number();
    return tmpP;
}

void get_limit() {
    for (auto x : Sec0) {
        if (x.op == "const" and x.arg1 == "limit") {
            haveLimit = true;
            limit = stoi(x.arg2);
            return;
        }
    }
}

TreeNode* parse_vardecl() {
    TreeNode* tmpP = NULL;
    if (get_str(tokens[index]) == "var") {
        node nVar = tokens[index];
        match("var");
        Sec0.push_back(section{ "var", get_str(tokens[index]), "0", "-" });
        tmpP = parse_ident();

        while (get_str(tokens[index]) == ",") {
            node d = tokens[index];
            match(",");
            Sec0.push_back(section{ "var", get_str(tokens[index]), "0", "-" });
            tmpP = opr(d, 2, vector<TreeNode*>{tmpP, parse_ident()});
        }
        tmpP = opr(nVar, 1, vector<TreeNode*>{tmpP});
        match(";");
    }
    return tmpP;
}

void get_ans() {
    n = limit;
    f2 = find(limit);
    f1 = find(limit - 1);
    t = f1;
    haveAns = true;
}

TreeNode* parse_procdecl() {
    TreeNode* tmpP = NULL;
    while (get_str(tokens[index]) == "procedure") {
        node Procedure = tokens[index];
        match("procedure");
        TreeNode* tmpPN = id(tokens[index]);
        parse_ident();
        match(";");
        auto vectmpPB = parse_block();
        TreeNode* tmpPB = opr(nEndD, 4, vectmpPB);
        node f = tokens[index];
        match(";");
        auto tp = opr(Procedure, 2, vector<TreeNode*> {tmpPN, tmpPB});
        if (tmpP == NULL) {
            tmpP = tp;
        }
        else {
            tmpP = opr(f, 2, vector<TreeNode*>{tmpP, tp});
        }
    }
    return tmpP;
}

TreeNode* parse_statement() {
    TreeNode* tmpP = NULL;
    if (identify_map.count(get_str(tokens[index]))) {
        tmpP = parse_assignstmt();
    }
    else if (get_str(tokens[index]) == "call") {
        tmpP = parse_callstmt();
    }
    else if (get_str(tokens[index]) == "begin") {
        tmpP = parse_compstmt();
    }
    else if (get_str(tokens[index]) == "if") {
        tmpP = parse_ifstmt();
    }
    else if (get_str(tokens[index]) == "while") {
        tmpP = parse_whilestmt();
    }
    if (!haveLimit) get_limit();
    if (!haveAns) get_ans();
    return tmpP;
}

TreeNode* parse_assignstmt() {
    auto nowIdent = get_str(tokens[index]);
    auto tmpP = parse_ident();
    node mEqual = tokens[index];
    match(":=");
    auto Rtexpression = parse_expression();
    string RtName = Rtexpression->tempVarName;
    if (RtName == "-") RtName = get_str(node{ Rtexpression->type, Rtexpression->val });
    Sec.push_back(section{ ":=", RtName, "-", nowIdent }); // Tag1
    tmpP = opr(mEqual, 2, vector<TreeNode*>{tmpP, Rtexpression});
    return tmpP;
}

TreeNode* parse_callstmt() {
    TreeNode* tmpP = NULL;
    node Call = tokens[index];
    match("call");
    auto ParseIdent = parse_ident();
    Sec.push_back(section{ "call", "-", "-", "print" });
    tmpP = opr(Call, 1, vector<TreeNode*>{ParseIdent});
    return tmpP;
}

TreeNode* parse_compstmt() {
    node nBegin = tokens[index];
    TreeNode* tmpP = NULL;
    match("begin");
    tmpP = parse_statement();
    while (get_str(tokens[index]) == ";") {
        auto F = tokens[index];
        match(";");
        tmpP = opr(F, 2, vector<TreeNode*>{tmpP, parse_statement()});
    }
    tmpP = opr(nBegin, 1, vector<TreeNode*>{tmpP});
    match("end");
    return tmpP;
}

TreeNode* parse_ifstmt() {
    TreeNode* tmpP = NULL;
    auto nIf = tokens[index];
    match("if");
    tmpP = parse_condition();
    match("then");
    tmpP = opr(nIf, 2, vector<TreeNode*>{tmpP, parse_statement()});
    return tmpP;
}

TreeNode* parse_whilestmt() {
    TreeNode* tmpP = NULL;
    auto nWhile = tokens[index];
    match("while");
    tmpP = parse_condition();
    match("do");
    tmpP = opr(nWhile, 2, vector<TreeNode*>{tmpP, parse_statement()});
    return tmpP;
}

TreeNode* parse_condition() {
    TreeNode* tmpP = NULL;
    if (get_str(tokens[index]) == "odd") {
        auto nOdd = tokens[index];
        match("odd");
        tmpP = opr(nOdd, 1, vector<TreeNode*>{parse_expression()});
    }
    else {
        tmpP = parse_expression();
        node nSymbol = tokens[index];
        if (match("", '#')) {// Tag4
            // # (!=)
            auto ntmp = parse_expression();
            string tmpName = tmpP->tempVarName;
            if (tmpName == "-") tmpName = get_str(node{ tmpP->type, tmpP->val });
            string ntmpName = ntmp->tempVarName;
            if (ntmpName == "-") ntmpName = get_str(node{ ntmp->type, ntmp->val });
            Sec.push_back(section{ "j#", tmpName, ntmpName, "106" });
            Sec.push_back(section{ "j", "-", "-", "113" });
        }
        else {
            match("", true);
            tmpP = opr(nSymbol, 2, vector<TreeNode*>{tmpP, parse_expression()});
        }

    }
    return tmpP;
}

TreeNode* parse_expression() { // Tag2 
    auto tmpP = parse_term();
    while (get_str(tokens[index]) == "+" || get_str(tokens[index]) == "-") {
        node nOp = tokens[index];
        match(get_str(tokens[index]));
        auto Rt = parse_term();
        string tmpPname = tmpP->tempVarName;
        if (tmpPname == "-") tmpPname = get_str(node{ tmpP->type, tmpP->val });
        string Rtname = Rt->tempVarName;
        if (Rtname == "-") Rtname = get_str(node{ Rt->type, Rt->val });
        Sec.push_back(section{ get_str(nOp), tmpPname, Rtname, getTempNumber() });
        // cout << Sec.back().result << " ::\n";
        tmpP = opr(nOp, 2, vector<TreeNode*>{tmpP, Rt}, Sec.back().result);
    }
    // cout << tmpP->tempVarName << "  ppp\n";
    return tmpP;
}

TreeNode* parse_term() { // Tag3
    auto tmpP = parse_factor();
    while (get_str(tokens[index]) == "*" || get_str(tokens[index]) == "/") {
        string name = getTempNumber();
        node nOp = tokens[index];
        match(get_str(tokens[index]));
        auto Rt = parse_factor();
        string tmpPname = tmpP->tempVarName;
        if (tmpPname == "-") tmpPname = get_str(node{ tmpP->type, tmpP->val });
        string Rtname = Rt->tempVarName;
        if (Rtname == "-") Rtname = get_str(node{ Rt->type, Rt->val });
        Sec.push_back(section{ get_str(nOp), tmpPname, Rtname, getTempNumber() });
        // cout << Sec.back().result << "  result\n";
        tmpP = opr(nOp, 2, vector<TreeNode*>{tmpP, Rt}, Sec.back().result);
    }
    // cout << tmpP->tempVarName << " :::\n";
    return tmpP;
}



TreeNode* parse_factor() {
    node sign{ -1, "" };
    TreeNode* tmpP = NULL;
    if (get_str(tokens[index]) == "+" || get_str(tokens[index]) == "-") {
        sign = tokens[index];
        match(get_str(tokens[index]));
    }
    if (isDigit(get_str(tokens[index]))) {
        tmpP = num(tokens[index]);
        parse_number();
    }
    else if (identify_map.count(get_str(tokens[index]))) {
        tmpP = id(tokens[index]);
        parse_ident();
    }
    else {
        match("(");
        tmpP = parse_expression();
        match(")");
    }
    if (sign.type != -1) {
        string tmpPname = tmpP->tempVarName;
        if (tmpPname == "-") tmpPname = get_str(node{ tmpP->type, tmpP->val });
        Sec.push_back(section{ "neg", tmpPname, "-", getTempNumber() });
        tmpP = opr(sign, 1, vector<TreeNode*>{tmpP}, Sec.back().result);
    }
    else {
        tmpP->tempVarName = get_str(node{ tmpP->type, tmpP->val });
    }
    return tmpP;
}

TreeNode* parse_ident() {
    TreeNode* tmpP = NULL;
    if (identify_map.count(get_str(tokens[index]))) {
        tmpP = id(tokens[index]);
        match(get_str(tokens[index]));
    }
    else {
        exception_print(1);
    }
    return tmpP;
}

TreeNode* parse_number() {
    TreeNode* tmpP = NULL;
    if (isDigit(get_str(tokens[index]))) {
        tmpP = num(tokens[index]);
        match(get_str(tokens[index]));
    }
    else {
        exception_print(2);
    }
    return tmpP;
}


bool GrammaticalAnalysis() {
    root = parse_program();
    if (success) {
        cout << "\n\n分析结果:  语法正确" << "\n";
    }
    return success;
}

void dfs(TreeNode* rt, int Tab) { // print abstract grammar tree
    if (rt == NULL) return;
    for (int i = 0; i < Tab; i++) cout << string(2, ' ');
    cout << get_str(node{ rt->type, rt->val }) << "\n";
    for (auto p : rt->son) {
        dfs(p, Tab + 1);
    }
}

int nowNumber = 100;

void print(section p) {
    cout << nowNumber++ << " ( ";
    cout << setw(3) << p.op << ", ";
    cout << setw(3) << p.arg1 << ", ";
    cout << setw(3) << p.arg2 << ", ";
    cout << setw(3) << p.result << ")\n";
}

void print0(section p) {
    cout << "(";
    cout << setw(5) << p.op << ", ";
    cout << setw(5) << p.arg1 << ", ";
    cout << setw(5) << p.arg2 << ", ";
    cout << setw(5) << p.result << ")\n";
}

vector<string> readFileToVector(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
    }

    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    return lines;
}

// 主函数定义
int main(int argc, char* argv[]) {
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

    vector<string> str;
    read(str);

    init();
    LexicalAnalysis(str, tokens);
    GrammaticalAnalysis();

    cout << "n = " << n << "\n";
    cout << "limit = " << limit << "\n";
    cout << "f1 = " << f1 << "\n";
    cout << "f2 = " << f2 << "\n";
    cout << "t = " << t << "\n";

    cout << "\n\n";
    cout << "语法分析树为: \n";
    dfs(root, 0);

    cout << "\n\n";
    cout << "section .data\n";
    for (auto x : Sec0) {
        print0(x);
    }
    cout << "\n";

    cout << "section .code\n";
    for (auto x : Sec) {
        print(x);
    }

    return 0;
}
