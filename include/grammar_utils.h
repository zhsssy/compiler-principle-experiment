//
// Created by zhsssy.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_GRAMMAR_UTILS_H
#define COMPILER_PRINCIPLE_EXPERIMENT_GRAMMAR_UTILS_H

#include <structure_ll1.h>
#include <fstream>
#include <string>
#include <iostream>


grammar create_grammar_by_file(char *filename);

void deal_with_left(grammar &g, const std::string &right, bool &first);

void deal_with_right(grammar &g, VN_TYPE vn, const std::string &right);

void write_grammar_in_file(grammar g, char *filename);

grammar create_grammar_by_file(char *filename) {
    grammar g;
    std::ifstream fin = std::ifstream(filename, std::ios_base::in);
    std::string s;
    bool first = true;

    if (!fin.is_open()) {
        std::cerr << "Can't open " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(fin, s)) {
        size_t index = s.find("->");

        // 没有 ‘->’ 退出
        if (index == std::string::npos) {
            std::cerr << "ERROR::" << s << "::NOT\"->\"." << std::endl;
            continue;
        }

        // 没有 ‘;’ 退出
        if (*(--s.cend()) != ';') {
            std::cerr << "ERROR::" << s << "::NOT \";\"." << std::endl;
        }

        // 删除 ‘;’
        s.erase(s.size() - 1);

        // 划分 ‘->’ 两边 | left 长度不止为1
        string left = s.substr(0, index);
        string right = s.substr(index + 2);

        deal_with_left(g, left, first);
        deal_with_right(g, left.at(0), right);
    }

    return g;
}

void deal_with_left(grammar &g, const std::string &left, bool &first) {
    if (first) {
        first = false;
        g.set_start(left.at(0));
    }

    g.add_vn(left.at(0));
}

void deal_with_right(grammar &g, const VN_TYPE vn, const std::string &right) {
    std::string::const_iterator i;
    std::string::const_iterator j;

    for (i = right.cbegin(), j = right.cbegin(); i != right.cend();) {
        // 分别加入
        if (*i == '|') {
            g.add_production(vn, std::string(j, i));
            j = ++i;
        } else {
            if (isupper(*i))
                g.add_vn(*i);
            else
                g.add_vt(*i);
            ++i;
        }
    }

    if (j != i)
        g.add_production(vn, std::string(j, i));
}

void write_grammar_in_file(const grammar g, char *filename) {
    std::ofstream fout(filename);

    if (!fout.is_open()) {
        std::cerr << "Can't open " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    for (ProPair pair: g.get_production()) {
        string tmp;
        tmp += pair.first;
        tmp += "->";

        for (const string &s: pair.second) {
            tmp += s + "|";
        }
        tmp.back() = ';';
        fout << tmp << endl;
    }
}

#endif //COMPILER_PRINCIPLE_EXPERIMENT_GRAMMAR_UTILS_H
