//
// Created by zhsssy on 2021.11.28.
//

#include <structure_ll1.h>

#include <grammar_utils.h>
#include <iostream>

using std::cout;
using std::endl;
using std::set;
using std::string;


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    grammar g = create_grammar_by_file(argv[1]);

    g.construct_LL1();

    cout << "The result:" << endl;

    for (VN_TYPE vn: g.get_vn()) {
        cout << vn << " ";
    }

    cout << endl;

    for (ProPair pair: g.get_production()) {
        string tmp;
        tmp += pair.first;
        tmp += "->";

        for (const auto &s: pair.second) {
            tmp += s + "|";
        }
        tmp.back() = ';';
        cout << tmp << endl;
    }

    write_grammar_in_file(g, argv[2]);
}