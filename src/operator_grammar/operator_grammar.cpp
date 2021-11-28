//
// Created by zhsssy on 2021/11/26.
//

#include "operator_grammar.h"


int main() {
    OperatorProgrammer o;
    o.init();
    o.judge();

    cout << o.get_result() << endl;

    return 0;
}