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
                    throw ILLEGAL_OPEN_BRACKET;
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
                    throw MISSING_OPERATOR;
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
                    throw MISSING_OPERATOR;
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
