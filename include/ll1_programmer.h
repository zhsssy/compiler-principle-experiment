//
// Created by zhsssy on 2021.11.15.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
#define COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H

#include <string>
#include <stack>
#include <iostream>
#include <fstream>
#include <lexical_analysis.h>
#include <queue>

using namespace std;

class LL1Programmer {
private :
    stack<string> symbol;
    queue<string> input;
    LEXICAL_RESULT result;
    string answer;


public:
    explicit LL1Programmer()
            : result(ERROR) { clear_stack(); }


    // 调取词法分析器
    void next() {
        LexicalAnalysis l("../resource/expression.txt");
        if (!l.is_open()) {
            cout << "Can't open file." << endl;
            exit(EXIT_FAILURE);
        }
        while (l.next()) {
            result = l.get_result();
            if (l.get_token() == ";") {
                // 到句末后开始判断 当前是否为符合文法的句子
                // TODO: judge() 逻辑部分存在问题 死循环
                input.push("#");
                queue<string> temp = input;
                cout << "start to print input" << endl;
                while (!temp.empty()) {
                    // debug： 排除 # 和 ; 不打印
                    if (temp.front() == "#" && temp.front() == ";") {
                        break;
                    }
                    cout << temp.front();
                    temp.pop();
                }
                cout << endl;
                judge();
                // 输出结果
                // 清空 input 和 symbol 栈 开始下一次
                clear_stack();
                cout << endl;
            }

            // 保存临时结果  判断推入 input 栈中的词法类型
            LEXICAL_RESULT la_temp = l.get_result();
            // 将标识符和数字转换为 i ，便于分析
            if (is_input(la_temp))
                input.push("i");

            if (is_key(la_temp) && l.get_token() != ";")
                input.push(l.get_token());
        }
    }

    // 逐句读取 -》 通过词法分析器转换为 LEXICAL_RESULT 组成的标识符 -》输入 stack 判断
    // 数字 / 小写字母   转换为 i 便于分析
    // 给定符号串 判定输入串 r result
    // TODO: 简化代码

    // debug
    void print_stack() {
        stack<string> temp_symbol = symbol;
        queue<string> temp_input = input;
        cout << "symbol: ";
        while (!temp_symbol.empty()) {
            cout << temp_symbol.top();
            temp_symbol.pop();
        }
        cout << endl;
        cout << "input: ";
        while (!temp_input.empty()) {
            cout << temp_input.front();
            temp_input.pop();
        }
        cout << endl;
    }

    void judge() {
        while (!symbol.empty()) {
            if (symbol.top() == "#") {
                if (input.front() == "#") {
//                    this->answer = "success";
                    cout << "success" << endl;
                    break;
                } else {
                    cout << "failed" << endl;
//                    this->answer = "failed";
                    break;
                }
            }
            if (symbol.top() == "E" && (input.front() == "i" || input.front() == "(")) {
                symbol.pop();
                symbol.push("E1");
                symbol.push("T");
                print_stack();
            }
            if (symbol.top() == "E1") {
                if (input.front() == "+") {
                    symbol.pop();
                    input.pop();
                    symbol.push("E1");
                    symbol.push("T");
                    print_stack();
                }
                if (input.front() == ")" || input.front() == "#") {
                    symbol.pop();
                    if (symbol.size() > 1 && input.size() > 1 && symbol.top() == input.front()) {
                        symbol.pop(), input.pop();
                    }
                    print_stack();
                }
            }
            if (symbol.top() == "T" && (input.front() == "i" || input.front() == "(")) {
                symbol.pop();
                symbol.push("T1");
                symbol.push("F");
                print_stack();
            }
            if (symbol.top() == "T1") {
                if (input.front() == "*") {
                    symbol.pop();
                    input.pop();
                    symbol.push("T1");
                    symbol.push("F");
                    print_stack();
                }
                if (input.front() == ")" || input.front() == "#" || input.front() == "+") {
                    symbol.pop();
                    if (symbol.size() > 1 && input.size() > 1 && symbol.top() == input.front()) {
                        symbol.pop(), input.pop();
                    }
                    print_stack();
                }
            }
            if (symbol.top() == "F") {
                if (input.front() == "i") {
                    symbol.pop();
                    input.pop();
                    print_stack();
                }
                if (input.front() == "(") {
                    symbol.pop();
                    input.pop();
                    symbol.push(")");
                    symbol.push("E");
                    print_stack();
                }
                if (input.front() == "#") {
                    string strs = symbol.top();
                    cout << "strs is " << strs << endl;
                    if (is_symbol_top_key(strs)) {
                        cout << "failed" << endl;
                        break;
                    }
                }

            }
        }
        // 根据 ll1 分析表 推出
    }

    static bool is_symbol_top_key(const string &a) {
        cout << "now start compare" << endl;
        return a == "(" || a == ")" || a == "+" || a == "-" || a == "*" || a == "/";
    }

    // 10进制 8进制 16进制 标识符
    static bool is_input(LEXICAL_RESULT r) {
        return r == LEXICAL_RESULT::HEX
               || r == LEXICAL_RESULT::OCT
               || r == LEXICAL_RESULT::DIGIT
               || r == LEXICAL_RESULT::IDENTIFICATION;
    }

    // 为 运算符 或 界符
    static bool is_key(LEXICAL_RESULT r) {
        return r == LEXICAL_RESULT::KEY;
    }

    // 清空 stack 开始下个句子分析
    void clear_stack() {
        queue<std::string>().swap(input);
        stack<std::string>().swap(symbol);
        symbol.push("#");
        symbol.push("E");
    }

    string get_answer() {
        return answer;
    }

};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
