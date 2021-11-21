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
    stack<string> symbol_stack;
    stack<string> input_stack;
    LEXICAL_RESULT result;
    string answer;


public:
    explicit LL1Programmer()
            : result(ERROR) { clear_stack(); }


    // 调取词法分析器
    void next() {
//        LexicalAnalysis lexical_analysis("../resource/expression.txt");
        LexicalAnalysis l("../resource/expression.txt");
        if (!l.is_open()) {
            cout << "Can't open file." << endl;
            exit(EXIT_FAILURE);
        }
        while (l.next()) {
            result = l.get_result();
//            cout << "the result is" << result << endl;
            if (l.get_token() == ";") {
//                // 到句末后开始判断 当前是否为符合文法的句子
//                // TODO: judge() 逻辑部分存在问题 死循环
//                judge();
//                // 输出结果
//                cout << "answer is " << answer << endl;
//                // 清空 input 和 symbol 栈 开始下一次
                while (!input_stack.empty()) {
//                    cout << "input stack :" << input_stack.top() << endl;
                    if (input_stack.top() == "#") {
                        break;
                    }
                    cout << input_stack.top();
                    input_stack.pop();
                }
//                while (!symbol_stack.empty()) {
////                    cout << "symbol_stack:" << symbol_stack.top() << endl;
//                    if (symbol_stack.top() == "#") {
//                        break;
//                    }
//                    cout << symbol_stack.top();
//                    symbol_stack.pop();
//                }
                clear_stack();
//                continue;
                cout << endl;
//                cout << "next time !!! " << endl;
            }
            // 保存临时结果  判断推入 input_stack 栈中的词法类型
            LEXICAL_RESULT la_temp = l.get_result();
            // 将数字转换为 i ，便于分析
            if (is_input(la_temp)) {
                input_stack.push("i");
            }
            if (is_key(la_temp)) {
                input_stack.push(l.get_token());
            }
        }
    }

    // 逐句读取 -》 通过词法分析器转换为 LEXICAL_RESULT 组成的标识符 -》输入 stack 判断
    // 数字 / 小写字母   转换为 i 便于分析
    // 给定符号串 判定输入串 r result
    // TODO: 简化代码

    void pop_same() {
        while (symbol_stack.top() == input_stack.top())
            symbol_stack.pop(), input_stack.pop();
    }

    void judge() {
        while (symbol_stack.top() != "#") {
            if (symbol_stack.top() == "E" && (input_stack.top() == "i" || input_stack.top() == "(")) {
                symbol_stack.pop();
                symbol_stack.push("E1");
                symbol_stack.push("T");
                pop_same();
            }
            if (symbol_stack.top() == "E1") {
                if (input_stack.top() == "+") {
                    symbol_stack.pop();
                    symbol_stack.push("E1");
                    symbol_stack.push("T");
                    symbol_stack.push("+");
                    pop_same();
                }
                if (input_stack.top() == "(" || input_stack.top() == "#") {
                    symbol_stack.pop();
                    pop_same();
                }
            }
            if (symbol_stack.top() == "T" && (input_stack.top() == "i" || input_stack.top() == "(")) {
                symbol_stack.pop();
                symbol_stack.push("T1");
                symbol_stack.push("F");
                pop_same();
            }
            if (symbol_stack.top() == "T1") {
                if (input_stack.top() == "*") {
                    symbol_stack.pop();
                    symbol_stack.push("T1");
                    symbol_stack.push("F");
                    symbol_stack.push("*");
                    pop_same();
                }
                if (input_stack.top() == ")" || input_stack.top() == "#" || input_stack.top() == "+") {
                    symbol_stack.pop();
                    pop_same();
                }
            }
            if (symbol_stack.top() == "F") {
                if (input_stack.top() == "i") {
                    symbol_stack.pop();
                    symbol_stack.push("i");
                    pop_same();
                }
                if (input_stack.top() == "(") {
                    symbol_stack.pop();
                    symbol_stack.push(")");
                    symbol_stack.push("E");
                    symbol_stack.push("(");
                    pop_same();
                }
            }
            // 同时推出相同的符号 （标识符） i + - * / ( )

            if (symbol_stack.empty() && input_stack.empty()) {
                this->answer = "成功";
                break;
            }
        }
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
        input_stack.push("#");
        symbol_stack.push("#");
        symbol_stack.push("E");
    }

    string get_answer() {
        return answer;
    }

};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
