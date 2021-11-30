//
// Created by zhsssy.
//

#include <ll1_analysis.h>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    LL1Analysis l(argv[1]);

    l.make_first();
    l.make_follow();
    l.make_table();
#ifdef DEBUG
    l.print_first();
    l.print_follow();
    l.print_table();
#endif
    l.analysis();

}