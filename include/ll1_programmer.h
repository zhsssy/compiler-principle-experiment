//
// Created by zhsssy on 2021.11.15.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
#define COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H

#include <string>
#include <stack>

class LL1Programmer {
    std::stack<std::string> symbol_stack;
    std::stack<std::string> input_stack;

    void result(std::string s) {
        symbol_stack.push("E");

    }
};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LL1_PROGRAMMER_H
