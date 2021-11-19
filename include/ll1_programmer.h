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

using namespace std;

class LL1Programmer {
private :
    std::ifstream fin;
    std::stack<string> symbol_stack;
    std::stack<string> input_stack;
    LEXICAL_RESULT result;
    string answer;
    char *p{};


public:
    explicit LL1Programmer(const string &filename)
            : fin(filename, ios_base::in), result(ERROR) {}

    string get_answer() {
        return answer;
    }

    // 调取词法分析
    bool next() {
        clear_stack();
        return false;
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
        stack<std::string>().swap(input_stack);
        stack<std::string>().swap(symbol_stack);
    }

    void init_input_stack() {

    }

    // 逐句读取 -》 通过词法分析器转换为 LEXICAL_RESULT 组成的标识符 -》输入 stack 判断
    // 数字 / 小写字母   转换为 i 便于分析
    // 给定符号串 判定输入串 r result
    void judge(const string &token, LEXICAL_RESULT r) {
        if (symbol_stack.top() == "E" && (is_input(result) || (is_key(result) && token == "("))) {
            symbol_stack.pop();
            symbol_stack.push("E1");
            symbol_stack.push("T");
        }
        if (symbol_stack.top() == "E1") {
            if (token == "+") {
                symbol_stack.pop();
                symbol_stack.push("E1");
                symbol_stack.push("T");
                symbol_stack.push("+");
            }
            if (token == "(" || token == "#") {
                symbol_stack.pop();
            }
        }
        if (symbol_stack.top() == "T" && (is_input(result) || (is_key(result) && token == "("))) {
            symbol_stack.push("T1");
            symbol_stack.push("F");
        }
        if (symbol_stack.top() == "T1") {
            if (token == "*") {
                symbol_stack.pop();
                symbol_stack.push("T1");
                symbol_stack.push("F");
                symbol_stack.push("*");
            }
            if (token == ")" || token == "#" || token == "+") {
                symbol_stack.pop();
            }
        }
        if (symbol_stack.top() == "F") {
            if (is_input(result)) {
                symbol_stack.pop();
                symbol_stack.push(token);
            }
            if (token == "(") {
                symbol_stack.pop();
                symbol_stack.push(")");
                symbol_stack.push("E");
                symbol_stack.push("(");
            }
        }
    }


};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
