//
// Created by zhsssy on 2021.11.15.
//

#include <ll1_programmer.h>
#include <lexical_analysis.h>

int main() {
    LL1Programmer ll1("../resource/expression.txt");

    while (ll1.next()) {
        cout << ll1.get_answer() << endl;
    }
}