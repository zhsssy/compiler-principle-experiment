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

class OperatorProgrammer {
public:
    void judge() {
        grammar g = create_grammar_by_file("");
        LexicalAnalysis l("");
        unsigned k = 0;
        unsigned index = 0;
        char token, tmp, N;
        string stack = "#";
        std::stack<string> polish;
        string str_tmp;
        bool next = 0;

        if (!l.is_open()) {
            cerr << "Input filename isn't exist!" << endl;
            exit(EXIT_FAILURE);
        }

        do {
            next = l.scan();
            str_tmp += l.get_token();

            switch (l.get_result()) {
                case LEXICAL_RESULT::IDENTIFICATION:
                case LEXICAL_RESULT::DIGIT:
                case LEXICAL_RESULT::OCT:
                case LEXICAL_RESULT::HEX:
                    token = 'i';
                    break;
                case LEXICAL_RESULT::KEY:
                case LEXICAL_RESULT::END:
                    token = l.get_token().at(0);
                    break;
                case LEXICAL_RESULT::ERROR:
                default:
                    cerr << "出现无效字符： " << l.get_token() << endl;
                    continue;
            }

            if (!next && str_tmp.size() == 1) break;

            if (token == ';') token = '#';

            if (stack.at(k) == '#' || g.is_vt(stack.at(k))) index = k;
            else index = k - 1;

            try {
                while (priority_judgement(stack.at(index), token) == 1) {
                    do {
                        tmp = stack.at(index);
                        if (stack.at(index - 1) == '#' || g.is_vt(stack.at(index - 1))) --index;
                        else index -= 2;
                    } while (priority_judgement(stack.at(index), tmp) != -1);
                    const string t = stack.substr(index + 1, k);

                    N = g.fuzzy_reduced(t);
                    trans_for_polish(t, polish);
                    stack.erase(index + 1, k - index - 1);
                    k = index + 1;
                    stack.at(k) = N;
                }
            }
            catch (const char *e) {
                cerr << e << endl;
            }
            catch (const ReduceException e) {
                deal_with_reduce_error(stack.substr(index + 1, k), g);
            }

            try {
                if (priority_judgement(stack.at(index), token) != 1) {
                    ++k;
                    stack += token;

                    if (token != '#') {
                        switch (l.get_result()) {
                            case LEXICAL_RESULT::IDENTIFICATION:
                                throw "不支持变量！";
                            case LEXICAL_RESULT::KEY:
                            case LEXICAL_RESULT::DIGIT:
                                polish.push(l.get_token());
                                break;
                            case LEXICAL_RESULT::OCT:
                                polish.push(std::to_string(std::stoi(l.get_token(), nullptr, 8)));
                                break;
                            case LEXICAL_RESULT::HEX:
                                polish.push(std::to_string(std::stoi(l.get_token(), nullptr, 16)));
                                break;
                        }
                    }
                } else {
                    cerr << "ERROR::AT '" << l.get_token() << "'\n";
                }
            }
            catch (const char *e) {
                cerr << e << endl;
            }

            if (token == '#') {
                cout << "字符串： " << str_tmp << " 判断结束\n";

                if (polish.size() == 1)
                    cout << "计算结果为：" << compute_polish(polish.top()) << endl;
                else
                    cout << "波兰表达式有误！" << endl;

                while (!polish.empty())
                    polish.pop();

                str_tmp = "";
            }

        } while (next);
    }

    static void trans_for_polish(const string &str, stack <string> &stack) {
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

    static int compute_polish(const string &str) {
        string tmp;
        std::stack<int> num;

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

    static void deal_with_reduce_error(const string &str, const grammar &g) {
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

    static int priority_judgement(int first, int second) {
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
                        throw UNKNOWN_ERROR;
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
                        throw UNKNOWN_ERROR;
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
                        throw ILLEGAL_OPEN_BRACKET;
                    default:
                        throw UNKNOWN_ERROR;
                }
            case ')':
                switch (second) {
                    case '+':
                    case '*':
                    case ')':
                    case '#':
                        return 1;
                    case '(':
                        throw MISSING_OPERATOR;
                    case 'i':
                        throw MISSING_OPERATOR;
                    default:
                        throw UNKNOWN_ERROR;
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
                        throw MISSING_OPERATOR;
                    default:
                        throw UNKNOWN_ERROR;
                }
            case '#':
                switch (second) {
                    case '+':
                    case '*':
                    case '(':
                    case 'i':
                        return -1;
                    case ')':
                        throw ILLEGAL_CLOSE_BRACKET;
                    case '#':
                        return 0;
                    default:
                        throw MISSING_EXPRESSION;
                }
            default:
                throw UNKNOWN_ERROR;
        }
    }
};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H
