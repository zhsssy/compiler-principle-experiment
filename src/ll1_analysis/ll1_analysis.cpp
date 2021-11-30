//
// Created by zhsssy on 2021/11/29.
//

#include <ll1_analysis.h>
#include <iostream>

int main(int argc, char **argv) {
//    if (argc != 2) {
//        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
//        exit(EXIT_FAILURE);
//    }

    LL1Analysis l;

    l.make_first();
    l.make_follow();
    l.make_table();
#define DEBUG
#ifdef DEBUG
    cout << endl;
    for (const auto &i: l.get_first()) {
        cout << i.first << " ";
        for (auto j: i.second) {
            cout << j << " ";
        }
        cout << endl;
    }
    for (const auto &i: l.get_follow()) {
        cout << i.first << " ";
        for (auto j: i.second) {
            cout << j << " ";
        }
        cout << endl;
    }
    for (auto i: l.get_analysis_table()) {
        cout << i.first << "=";
        for (const auto &j: i.second) {
            cout << j.first << " {";
            for (auto k: j.second) {
                cout << k;
            }
            cout << "}";
        }
        cout << endl;
    }
#endif
    l.analysis();

}