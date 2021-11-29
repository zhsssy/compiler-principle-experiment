//
// Created by zhsssy on 2021/11/29.
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
typedef map<VN_TYPE, map<VT_TYPE, string>> AnalysisTable;

class LL1Analysis {
private:
    stack<string> symbol;
    queue<string> input;
    bool if_end;
    bool answer;
    grammar g;
    First first;
    Follow follow;
    AnalysisTable analysis_table;

public:
    LL1Analysis() : if_end(false), answer(false) {
        g = create_grammar_by_file("../../../resource/test_utils.txt");
        g.construct_LL1();
    };

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

    void make_first() {
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


        //循环结束条件： 直到每一个FIRST(X)不再增大为止
        //3.对X→Y0Y1…….Yk(且Y0 ∈Vn), 反复使用以下直到每一个FIRST(X)不再增大为止
        // 临时存储 vn 个 first 集
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

    void make_follow() {
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

    void make_table() {
        string action;
        //纵轴遍历Vn
        for (auto it1: g.get_vn()) {
            //横轴遍历Vt
            for (auto it2: g.get_vt()) {
                action = "";
                //如果it2是it1的first集元素:it2∈first(it1)
                if (first[it1].contains(it2)) {
                    //本应有一步动作：查询it2是it1哪一个规则的first集元素
                    VN_TYPE it_vn = it1;
                    VT_TYPE it_vt = it2;
                    set<string> principle;
                    principle = g.get_production()[it_vn];

                    //剔除空产生式
                    int T = 0;
                    for (auto it = principle.begin(); T < principle.size(); it++, T++) {
                        if (*it == " ")
                            principle.erase(it);
                    }

                    if (principle.size() > 1) {
                        for (const auto &prin_it: principle) {
                            //空动作不管
                            if (prin_it == " ")
                                continue;
                            //产生式的第一个即为vt，则必定为其first集
                            if (prin_it.find(it_vt) != string::npos) {
                                action = prin_it;
                                break;
                            }

                        }
                    } else {
                        for (auto it: g.get_production())
                            for (auto it2: it.second)
                                if (it2[0] == it_vn)
                                    action = it2;
                        analysis_table.insert({it_vn, {{it_vt, action}}});
                    }
                }
                    //如果it2是it1的follow集元素:it2∈follow(it1),且若ε ∈ FIRST(it1)
                else if (first.at(it1).count('@') != 0 && follow.at(it1).count(it2) != 0) {
                    action = "pop";
                    analysis_table.insert({it1, {{it2, action}}});
                }
                    //对应分析表中的空白，即出错
                else {
                    action = "-1";
                    analysis_table.insert({it1, {{it2, action}}});
                }
            }//横轴遍历Vt

            //纵轴添加一个‘#’

            //添加条件：Vn->ε 且 ‘#’ ∈ follow(Vn)
            if (first.at(it1).count('@') != 0 && follow.at(it1).count('#') != 0) {
                action = "pop";
                analysis_table.insert({it1, {{'#', action}}});
            }
                //对应分析表中的空白，即出错
            else {
                action = "-1";
                analysis_table.insert({it1, {{'#', action}}});
            }
        }//纵轴遍历Vn
    }

    // 调取词法分析器
    void analysis() {
        LexicalAnalysis l("../../../resource/expression.txt");

        if (!l.is_open()) {
            cout << "Can't open file." << endl;
            exit(EXIT_FAILURE);
        }
        while (l.scan()) {
            if (l.get_token() == ";") {
                // 到句末后开始判断 当前是否为符合文法的句子
                input.push("#");
#ifdef DEBUG
                queue<string> temp = input;
                cout << "start to print input" << endl;
                while (!temp.empty()) {
                    // debug： 排除 # 和 ; 不打印
                    if (temp.front() == "#" && temp.front() == ";") {
                        break;
                    }
                    cout << temp.front();
                    temp.pop(); w
                }
                cout << endl << endl;
#endif
                cout << answer << endl;
                init();
            }

            // 保存临时结果  判断推入 input 栈中的词法类型
            LEXICAL_RESULT la_temp = l.get_result();
            // 将标识符和数字转换为 i ，便于分析
            if (LexicalAnalysis::is_input(la_temp))
                input.push("i");

            if (LexicalAnalysis::is_key(la_temp) && l.get_token() != ";")
                input.push(l.get_token());
        }
    }

    void init() {
        queue<std::string>().swap(input);
        stack<std::string>().swap(symbol);
        symbol.push("#");
        symbol.push("E");
        this->answer = false;
    }

};

#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_ANALYSIS_H
