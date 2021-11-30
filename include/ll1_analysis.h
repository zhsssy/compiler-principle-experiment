
//
// Created by zhsssy
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_LL1_ANALYSIS_H
#define COMPILER_PRINCIPLE_EXPERIMENT_LL1_ANALYSIS_H


#include <set>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <stack>
#include <queue>

#include <lexical_analysis.h>
#include <structure_ll1.h>
#include <grammar_utils.h>

using std::set;
using std::map;
using std::pair;
using std::string;
using std::stack;
using std::queue;

typedef map<VN_TYPE, set<char>> First;
typedef map<VN_TYPE, set<char>> Follow;
typedef multimap<VN_TYPE, map<VT_TYPE, string>> AnalysisTable;

class LL1Analysis {
private:
    stack<VN_TYPE> symbol;
    queue<VT_TYPE> input;
    bool if_end;
    bool answer;
    grammar g;
    First first;
    Follow follow;
    AnalysisTable analysis_table;

public:
    explicit LL1Analysis(char *filename) : if_end(false), answer(false) {
        g = create_grammar_by_file(filename);
        g.construct_LL1();
#ifdef DEBUG
        g.print_production();
#endif
    };

    void print_first();

    void print_follow();

    void print_table();

    ~LL1Analysis() = default;

    [[nodiscard]] First get_first() const {
        return first;
    }

    [[nodiscard]] Follow get_follow() const {
        return follow;
    }

    [[nodiscard]] AnalysisTable get_analysis_table() const {
        return analysis_table;
    }

    void make_first();

    void make_follow();

    void make_table();

    void analysis();

    void analysis_single();

    void print_symbol_input(stack<char> &a, queue<char> &b) {
        stack<char> temp_symbol = a;
        queue<char> temp_input = b;
        string temp_str;
        cout << endl << "symbol: ";
        while (!temp_symbol.empty()) {
            temp_str += temp_symbol.top();
            temp_symbol.pop();
        }
        reverse(temp_str.begin(), temp_str.end());
        cout << temp_str;
        cout << endl << "input: ";
        while (!temp_input.empty()) {
            cout << temp_input.front();
            temp_input.pop();
        }
    }

    void init() {
        queue<char>().swap(input);
        stack<char>().swap(symbol);
        symbol.push('#');
        symbol.push(g.get_start());
        this->answer = false;
    }

};

void LL1Analysis::print_first() {
    cout << "FIRST:" << endl;
    for (const auto &i: first) {
        printf("FIRST(%c)=", i.first);
        cout << "{";
        for (auto j: i.second) {
            cout << j << " ";
        }
        cout << "}" << endl;
    }
}

void LL1Analysis::print_follow() {
    cout << "FOLLOW:" << endl;
    for (const auto &i: follow) {
        printf("FOLLOW(%c)=", i.first);
        cout << "{";
        for (auto j: i.second) {
            cout << j << " ";
        }
        cout << "}" << endl;
    }
}

void LL1Analysis::print_table() {
    cout << "TABLE:" << endl;
    for (auto i: analysis_table) {
        cout << i.first << "-";
        for (const auto &j: i.second) {
            cout << j.first << " {" << i.first << "->";
            for (auto k: j.second)
                cout << k;
            cout << "}";
        }
        cout << endl;
    }
}

void LL1Analysis::make_first() {
    //初始化first集
    for (char it: g.get_vn()) {
        set<char> t;
        first.insert(make_pair(it, t));
    }

    //遍历规则rule
    //2.若X∈Vn,有X→aα,(a∈Vt)或/和X→ε 则a或/和ε∈FIRST(x)
    for (auto &it: g.get_production()) {
        //用temp记录it->first对应的first集
        set<char> temp;
        //left -> right
        set<string> right = it.second;
        char left = it.first;
        for (auto a: right) {
            // 从 vt 中查找 文法中第一个a[0] 是否存在 存在则加入 first
            if (g.get_vt().count(a[0]) != 0) {
                temp.insert(a[0]);
            }
            // X→ε 则ε∈FIRST(x)
            if (a == "@") {
                temp.insert('@');
            }
        }
        if (first[left].empty()) {
            first[left] = temp;
        } else
            first[left].insert(temp.begin(), temp.end());
    }


    //3.对X→Y0Y1…….Yk(且Y0 ∈Vn), 反复使用以下直到每一个FIRST(X)不再增大为止
    // 开辟大小为 vn.size() 的空间 临时存储 vn 个 first 集
    // 循环求解直到不再增大
    set<char> temp[g.get_vn().size()];
    size_t k = 0, first_size = 0;
    for (auto it = g.get_production().begin();; it++, k++) {
        //用temp[k]记录it->first对应的first集
        //set<char> temp;
        //left -> right
        set<string> right = it->second;
        char left = it->first;
        //3.对X→Y0Y1…….Yk(且Y0 ∈Vn), 反复使用以下直到每一个FIRST(X)不再增大为止
        bool flag = true;//检测连续含{ε}的Yj
        // 检测每个文法 如果为 vn 则找寻
        for (auto str: right) {
            for (int j = 0; j < str.size() && g.get_vn().contains(str[j]) && flag; j++) {
                // find FIRST(Yj) |  temp1 = FIRST(Yj)
                set<char> temp1 = first[str[j]];
                /*
                 ii 若Y0、Y1、Y2、……Y i-1 ∈Vn(2≤i ≤k) 且ε ∈FIRST(Y j) (0≤j ≤i-1)
                    则把FIRST(Yi )\{ε}元素加入FIRST(x)中
                 */
                if (j < str.size() - 1) {
                    //Y0...Yj-1含{ε}
                    if (temp1.contains('@')) {
                        temp1.erase('@');
                    }
                        //Yj不含{ε}
                    else {
                        flag = false;
                    }
                    //FIRST(Yj)\{ε}
                    temp[k].insert(temp1.begin(), temp1.end());
                }
                    /*
                     iii 若Y0、Y1、……Yk ∈Vn且ε ∈FIRST(Yj)(0≤j ≤k)
                        则把ε元素加入FIRST(x)中
                     */
                else {
                    temp[k].insert('@');
                }
            }
        }
        if (first[left].empty()) {
            first[left] = temp[k];
        } else
            first[left].insert(temp[k].begin(), temp[k].end());

        //跳出条件
        if (k == first.size() - 1) {
            size_t cnt = 0;
            //求和，观察first集有无增大
            for (int kk = 0; kk <= k; kk++) {
                cnt += temp[kk].size();
            }
            //和上一次大小一样，循环终止
            if (cnt == first_size)
                break;
                //重新循环
            else {
                k = 0;
                it = g.get_production().begin();
                first_size = cnt;
            }
        }
    }
}

void LL1Analysis::make_follow() {
    //初始化follow集
    for (auto it: g.get_vn()) {
        set<char> t;
        //令#∈FOLLOW(S) S为文法开始符号
        if (it == g.get_start())
            t.insert('#');
        follow.insert(make_pair(it, t));
    }

    //遍历规则rule
    //2.对A→ αBβ,且β ≠ε, 则将 FIRST(β)\{ε}加入FOLLOW(B)中
    for (auto &it: g.get_production()) {
        //left -> right
        set<string> right = it.second;
        char left = it.first;
        //对每个left遍历
        for (auto str: right) {
            //对left的每一个规则遍历
            for (int j = 0; j < str.size(); j++) {
                //找到B所在：str[j]
                if (g.get_vn().count(str[j]) != 0) {
                    set<char> tt;
                    //β属于Vt
                    if (g.get_vt().count(str[j + 1]) != 0) {
                        tt.insert(str[j + 1]);
                        follow.at(str[j]).insert(tt.begin(), tt.end());
                    }
                    //β属于Vn,FIRST(β)\{ε}加入FOLLOW(B)中
                    if (g.get_vn().count(str[j + 1]) != 0) {
                        tt = first.at(str[j + 1]);
                        tt.erase('@');
                        follow.at(str[j]).insert(tt.begin(), tt.end());
                    }

                }//找到B所在：str[j]
            }//对left的每一个规则遍历
        }//对每个left遍历
    }//遍历规则rule

    // 3.反复, 直至每一个FOLLOW(A)不再增大
    //   对A→ αB或A→ αBβ(且ε ∈ FIRST(β))
    //   则FOLLOW(A)中的全部元素加入FOLLOW(B)
    int k = 0;
    size_t follow_size = 0;//记录follow集的大小
    for (auto it = g.get_production().begin();; it++, k++) {
        //left -> right
        set<string> right = it->second;
        char left = it->first;
        //对每个left遍历(一个left对应多个规则)
        for (auto str: right) {
            //对left的每一个规则遍历（一个规则对应多个字符）
            for (int j = 0; j < str.size(); j++) {
                //找到B所在：str[j]
                if (g.get_vn().count(str[j]) != 0) {
                    set<char> tt;
                    //到规则的最后一个字符
                    if (j == str.size() - 1) {
                        //最后一个字符是Vn，即A→αB，则FOLLOW(A)中的全部元素加入FOLLOW(B)
                        tt = follow[left];
                        follow.at(str[j]).insert(tt.begin(), tt.end());
                    }
                        //没到最后一个,即A→ αBβ(且ε∈FIRST(β)) 这种情况
                    else {
                        bool flag = true;
                        //检索ε∈FIRST(β)是否成立
                        for (int jj = j + 1; jj < str.size() && flag; jj++) {
                            //str[jj]是Vn且包含ε
                            if (g.get_vn().contains(str[jj]) && first[str[jj]].contains('@')) {
                                continue;
                            }
                                //str[jj]不是Vn或不包含ε
                            else {
                                flag = false;
                            }
                        }
                        //A→ αBβ(且ε∈FIRST(β))情况满足
                        if (flag) {
                            tt = follow[left];
                            follow.at(str[j]).insert(tt.begin(), tt.end());
                        }
                    }
                }//找到B所在：str[j]
            }//对left的每一个规则遍历
        }//对每个left遍历

        //跳出条件
        if (k == follow.size() - 1) {
            size_t cnt = 0;
            for (auto &it1: follow) {
                cnt += it1.second.size();
            }
            //和上一次大小一样，循环终止
            if (cnt == follow_size)
                break;
                //重新循环
            else {
                k = 0;
                it = g.get_production().begin();
                follow_size = cnt;
            }
        }//跳出条件
    }
}

//    1. for  ∀ a ∈ FIRST(α)， 将 A -> α 填入 M [A, a ];
//    2. if（ε ∈ FIRST(α)） ∀ a ∈ FOLLOW (A) ， 将 A -> ε 填入 M [A, a ]; PS: 这里的 a 包括 #
void LL1Analysis::make_table() {
    string action;
    for (auto it_vn: g.get_vn()) {
        // 获取当前 vn 的文法规则
        set<string> principle = g.get_production()[it_vn];
        // 查找 a 的文法集
        for (auto p_it: principle) {
            // 如果是 a的首字母是 vn ，则找出 FIRST(α) 之后逐个加入
            if (isupper(p_it[0])) {
                // 找到 FIRST(α)
                auto temp_first = first[p_it[0]];
                for (auto a: temp_first) {
                    analysis_table.insert({it_vn, {{a, p_it}}});
                }
                // 如果 @ 属于 FIRST(a) 则加入 ∀a ∈ FOLLOW(a) 将A -> ε填入M[A, a ]
                if (temp_first.contains('@')) {
                    auto temp_follow = follow[it_vn];
                    for (auto a: temp_follow) {
                        analysis_table.insert({it_vn, {{a, "@"}}});
                    }
                }
            } else if (p_it[0] == '@') {
                auto temp_follow = follow[it_vn];
                for (auto a: temp_follow) {
                    analysis_table.insert({it_vn, {{a, "@"}}});
                }
            }
                // α 第一个字母是小写字母 ，直接加入
            else {
                analysis_table.insert({it_vn, {{p_it[0], p_it}}});
            }
        }
    }
}

// 调取词法分析器
void LL1Analysis::analysis() {
    LexicalAnalysis l("../../../resource/expression.txt");

    if (!l.is_open()) {
        cout << "Can't open file." << endl;
        exit(EXIT_FAILURE);
    }
    init();
    while (l.scan()) {
        if (l.get_token() == ";") {
            // 到句末后开始判断 当前是否为符合文法的句子
            input.push('#');
#ifdef DEBUG
            cout << "input str: ";
                queue<char> temp = input;
                while (!temp.empty()) {
                    // debug： 排除 # 和 ; 不打印
                    if (temp.front() == '#')
                        break;
                    cout << temp.front();
                    temp.pop();
                }
#endif
            analysis_single();
            cout << endl << (answer ? "success" : "failed");
            init();
        }

        // 保存临时结果  判断推入 input 栈中的词法类型
        LEXICAL_RESULT la_temp = l.get_result();
        // 将标识符和数字转换为 i ，便于分析
        if (LexicalAnalysis::is_input(la_temp))
            input.push('i');

        if (LexicalAnalysis::is_key(la_temp) && l.get_token() != ";")
            input.push(l.get_token().c_str()[0]);
    }
}


void LL1Analysis::analysis_single() {
    bool judge_if_find;
    while (!input.empty()) {
        if (analysis_table.contains(symbol.top())) {
#ifdef DEBUG
            print_symbol_input(symbol, input);
#endif
            if_end = false;
            judge_if_find = false;
            // multimap find iterator range(st,en)
            auto temp_it = analysis_table.equal_range(symbol.top());
            auto st = temp_it.first, en = temp_it.second;
            string str;
            for (auto it = st; it != en; ++it)
                if (it->second.contains(input.front())) {
                    judge_if_find = true;
                    str = it->second[input.front()];
                }

            if (!judge_if_find) {
                break;
            }

            // 反转插入 symbol 栈
            reverse(str.begin(), str.end());
            auto temp = str;
            if (temp[0] == '@') {
                symbol.pop();
                if_end = true;
            } else if (!temp.empty()) {
                symbol.pop();
                for (auto s: temp)
                    symbol.push(s);
                if_end = true;
            }
            // 如果同时为 # 则证明正确
            if (symbol.top() == '#' && input.front() == '#') {
                this->answer = true;
                break;
            }

            // 错误条件
            if (!isupper(symbol.top()) && input.front() != symbol.top()) {
                break;
            }
            // 相同则推出
            if (symbol.top() == input.front()) {
#ifdef DEBUG
                cout << endl << "pop: " << symbol.top() << endl;
#endif
                symbol.pop();
                input.pop();
                if_end = true;
            }
            if (!if_end)
                break;
        }
    }
}

#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_ANALYSIS_H
