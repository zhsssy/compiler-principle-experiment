//
// Created by zhsssy.
//

#include "operator_grammar.h"


int main() {
    OperatorProgrammer o;
    o.init();
    o.judge();

    cout << o.get_result() << endl;

    return 0;
}