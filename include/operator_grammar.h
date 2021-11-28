//
// Created by zhsssy on 2021/11/26.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H
#define COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H

#include <lexical_analysis.h>
#include <iostream>
#include <map>
#include <string>


using namespace std;

class OperatorProgrammer {
private:
    string result;
    map<string, vector<string>> operator_table;

public:
    OperatorProgrammer() = default;

    string get_result() {
        return result;
    }

    void operator_construct() {

    }

    void build_first_vt() {

    }

    void build_last_vt() {

    }

// debug
#ifdef DEBUG

    void print() {
        // TODO: 符号表
        // TODO: 当前输入栈
        // TODO: 当前符号栈
        // TODO: 判断符号和输入之间的关系
    }

#endif

    void init() {
        LexicalAnalysis l("../resource/expression.txt");

        while (l.next()) {
            cout << l.get_result() << endl;
            cout << l.get_token() << endl;
        }
    }

    void judge() {

    }
};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_OPERATOR_PROGRAMMER_H
