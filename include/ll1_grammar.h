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
    bool if_end;
    bool answer;

public:
    explicit LL1Programmer()
            : if_end(false), answer(false) { init(); }

    // 调取词法分析器
    void analysis() {
        LexicalAnalysis l("../resource/expression.txt");
        if (!l.is_open()) {
            cout << "Can't open file." << endl;
            exit(EXIT_FAILURE);
        }
        while (l.scan()) {
            if (l.get_token() == ";") {
                // 到句末后开始判断 当前是否为符合文法的句子
                input.push("#");
                //这部分代码用于 debug
#ifdef DEBUG
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
                cout << endl << endl;
#endif
                judge();
                // 清空 input 和 symbol 开始下一次
                cout << (get_answer() ? "success" : "failed") << endl;
                init();
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

    // debug
    void print_symbol_input() {
        stack<string> temp_symbol = symbol;
        queue<string> temp_input = input;
        cout << endl << "symbol: ";
        while (!temp_symbol.empty()) {
            cout << temp_symbol.top();
            temp_symbol.pop();
        }
        cout << endl << "input: ";
        while (!temp_input.empty()) {
            cout << temp_input.front();
            temp_input.pop();
        }
    }

    inline void pop_together() {
        input.pop(), symbol.pop();
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
    void init() {
        queue<std::string>().swap(input);
        stack<std::string>().swap(symbol);
        symbol.push("#");
        symbol.push("E");
        this->answer = false;
    }

    bool get_answer() const {
        return answer;
    }

    void judge() {
        while (!symbol.empty()) {
            if_end = false;
            if (symbol.top() == "#") {
                if (input.front() == "#") {
                    this->answer = true;
                    break;
                } else {
                    this->answer = false;
                    break;
                }
            }
            if (symbol.top() == "E" && (input.front() == "i" || input.front() == "(")) {
                symbol.pop();
                symbol.push("E'");
                symbol.push("T");
#ifdef DEBUG
                print_symbol_input();
#endif
                if_end = true;
            }
            if (symbol.top() == "E'") {
                if (input.front() == "+") {
                    pop_together();
                    symbol.push("E'");
                    symbol.push("T");
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
                if (input.front() == ")" || input.front() == "#") {
                    symbol.pop();
                    if (symbol.size() > 1 && input.size() > 1 && symbol.top() == input.front()) {
                        symbol.pop(), input.pop();
                    }
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
            }
            if (symbol.top() == "T" && (input.front() == "i" || input.front() == "(")) {
                symbol.pop();
                symbol.push("T'");
                symbol.push("F");
#ifdef DEBUG
                print_symbol_input();
#endif
                if_end = true;
            }
            if (symbol.top() == "T'") {
                if (input.front() == "*") {
                    pop_together();
                    symbol.push("T'");
                    symbol.push("F");
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
                if (input.front() == ")" || input.front() == "#" || input.front() == "+") {
                    symbol.pop();
                    if (symbol.size() > 1 && input.size() > 1 && symbol.top() == input.front()) {
                        symbol.pop(), input.pop();
                    }
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
            }
            if (symbol.top() == "F") {
                if (input.front() == "i") {
                    pop_together();
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
                if (input.front() == "(") {
                    pop_together();
                    symbol.push(")");
                    symbol.push("E");
#ifdef DEBUG
                    print_symbol_input();
#endif
                    if_end = true;
                }
            }

            if (!if_end) {
                cout << "failed" << endl;
                break;
            }
        }
        // 根据 ll1 分析表 推出
    }

};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
