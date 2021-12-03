//
// Created by zhsssy.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H
#define COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H

#include <iostream>
#include <map>
#include <string>
#include <stack>

#include <lexical_analysis.h>
#include <grammar_utils.h>

using namespace std;

static const char *const UNKNOWN_ERROR = "不认识的运算符";
static const char *const ILLEGAL_OPEN_BRACKET = "非法左括号";
static const char *const MISSING_OPERATOR = "缺少运算符";
static const char *const ILLEGAL_CLOSE_BRACKET = "非法右括号";
static const char *const MISSING_EXPRESSION = "缺少表达式";


void trans_for_polish(const string &str, stack<string> &stack) {
    string tmp;

    if (str.size() == 3) {
        if (str.at(0) == '(' && str.at(2) == ')') {
            stack.pop();
            while (stack.top().at(0) != '(') {
                tmp += stack.top();
                stack.pop();
            }

            stack.pop();
            stack.push(tmp);
            return;
        } else if (str.at(1) == '+') {

            while (stack.top().at(0) != '+') {
                tmp += stack.top();
                stack.pop();
            }

            stack.pop();

            tmp = stack.top() + " " + tmp + " +";
            stack.pop();
            stack.push(tmp);

            return;
        } else if (str.at(1) == '*') {
            while (stack.top().at(0) != '*') {
                tmp += stack.top();
                stack.pop();
            }

            stack.pop();

            tmp = stack.top() + " " + tmp + " *";
            stack.pop();
            stack.push(tmp);

            return;
        }
    } else if (str.size() == 1) {
        return;
    }
    throw ReduceException();
}

int compute_polish(const string &str) {
    string tmp;
    stack<int> num;

    for (char ch: str) {
        if (ch == ' ') {
            if (tmp.at(0) == '+') {
                const int v1 = num.top();
                num.pop();
                const int v2 = num.top();
                num.pop();

                num.push(v1 + v2);
            } else if (tmp.at(0) == '*') {
                const int v1 = num.top();
                num.pop();
                const int v2 = num.top();
                num.pop();

                num.push(v1 * v2);
            } else {
                num.push(std::stoi(tmp, nullptr, 10));
            }
            tmp = "";
        } else {
            tmp += ch;
        }
    }

    cout << tmp << endl;
    const int v1 = num.top();
    num.pop();
    const int v2 = num.top();
    num.pop();

    if (tmp.at(0) == '+') {
        return v1 + v2;
    } else if (tmp.at(0) == '*') {
        return v1 * v2;
    }

    throw "波兰表达式错误！";
}

void deal_with_reduce_error(const string &str, const grammar &g) {
    size_t i, j;
    if (
            (i = str.find('(')) != string::npos &&
            (j = str.find(')')) != string::npos &&
            j - i == 2 &&
            !g.is_vn(str.at(i + 1))
            )
        cerr << "括号间无表达式" << endl;
    else if (
            (i = str.find('i')) != string::npos &&
            (
                    (i > 0 && g.is_vn(str.at(i - 1))) ||
                    (i < str.size() - 1 && g.is_vn(str.at(i + 1)))
            )
            )
        cerr << "表达式间无运算符联结" << endl;
    else if (
            (
                    (i = str.find('*')) != string::npos ||
                    (i = str.find('+')) != string::npos
            ) &&
            (
                    i == 0 ||
                    i == str.size() - 1 ||
                    !g.is_vn(str.at(i - 1)) ||
                    !g.is_vn(str.at(i + 1))
            )
            )
        cerr << "缺表达式" << endl;
}

int priority_judgement(int first, int second) {
    switch (first) {
        case '+':
            switch (second) {
                case '+':
                case ')':
                case '#':
                    return 1;
                case '*':
                case '(':
                case 'i':
                    return -1;
                default:
                    throw MISSING_EXPRESSION;
            }
        case '*':
            switch (second) {
                case '+':
                case '*':
                case ')':
                case '#':
                    return 1;
                case '(':
                case 'i':
                    return -1;
                default:
                    throw MISSING_EXPRESSION;
            }
        case '(':
            switch (second) {
                case '+':
                case '*':
                case '(':
                case 'i':
                    return -1;
                case ')':
                    return 0;
                case '#':
                default:
                    throw MISSING_EXPRESSION;
            }
        case ')':
            switch (second) {
                case '+':
                case '*':
                case ')':
                case '#':
                    return 1;
                case '(':
                case 'i':
                default:
                    throw MISSING_EXPRESSION;
            }
        case 'i':
            switch (second) {
                case '+':
                case '*':
                case ')':
                case '#':
                    return 1;
                case '(':
                case 'i':
                default:
                    throw MISSING_EXPRESSION;
            }
        case '#':
            switch (second) {
                case '+':
                case '*':
                case '(':
                case 'i':
                    return -1;
                case '#':
                    return 0;
                case ')':
                default:
                    throw MISSING_EXPRESSION;
            }
        default:
            throw MISSING_EXPRESSION;
    }
}

#endif //COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H
