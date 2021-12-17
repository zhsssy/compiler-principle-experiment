//
// Created by zhsssy.
//

#include "operator_grammar.h"

#include <utility>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>

using namespace std;

// S SDT
class Node {
public:
    char sym{};
    int val{};
    bool isDigit{};

    Node() = default;

    Node(char sym) : sym(sym), isDigit(false) {}

    Node(char sym, int val) : sym(sym), val(val), isDigit(true) {}

    Node(char sym, bool isDigit) : sym(sym), val(0), isDigit(isDigit) {}

    Node(char sym, int val, bool isDigit) : sym(sym), val(val), isDigit(isDigit) {}
};

// 是否为可计算的数字
// functional
inline bool isNumber(const string &str) {
    return !any_of(
            str.cbegin(), str.cend(),
            [](char c) { return isdigit(c) == 0; });
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    grammar g = create_grammar_by_file(argv[1]);
    LexicalAnalysis l(argv[2]);
    g.print_production();
    cout << "VT: ";
    for (auto i: g.get_vt())
        cout << i << " ";
    cout << endl;

    unsigned int k, j;
    size_t sym;
    char Q;
    Node SYM;
    vector<Node> str_stack, nodes;
    vector<vector<Node>> inputs;
    bool if_digit = true;
    

    if (!l.is_open()) {
        cerr << "Input filename isn't exist!" << endl;
        exit(EXIT_FAILURE);
    }

    // 根据 ; 全部分割再判断
    while (l.scan()) {
        if (l.get_token().at(0) == ';') {
            nodes.emplace_back(Node('#', 0));
            inputs.push_back(nodes);
            nodes.clear();
        }
        if (LexicalAnalysis::is_input(l.get_result())) {
            // 值转换 使用 Node 构造栈
            // TODO: 针对不同进制的转换计算
            if (!isNumber(l.get_token()))
                nodes.emplace_back(Node('i', 0, false));
            else
                nodes.emplace_back(Node('i', stoi(l.get_token())));
        } else if (LexicalAnalysis::is_key(l.get_result()) && l.get_token() != ";")
            nodes.emplace_back(l.get_token().at(0));
    }

    for (const auto &input: inputs) {
        if_digit = true;
        // functional 查找是否可以计算
        if (std::any_of(
                input.cbegin(), input.cend(),
                [](Node node) { return node.sym == 'i' && !node.isDigit; }))
            if_digit = false;
        // 每轮循环初始化
        k = 0, str_stack.clear(), sym = 0;
        str_stack.emplace_back(Node('#', 0));
        do {
            SYM = input.at(sym++);
            if (str_stack.at(k).sym == '#' || g.is_vt(str_stack.at(k).sym)) j = k;
            else j = k - 1;

            try {
                while (priority_judgement(str_stack.at(j).sym, SYM.sym) == 1) {
                    do {
                        Q = str_stack.at(j).sym;
                        if (j <= 0) break;
                        if (j > 0 && (str_stack.at(j - 1).sym == '#' || g.is_vt(str_stack.at(j - 1).sym))) j = j - 1;
                        else j = j - 2;
                    } while (priority_judgement(str_stack.at(j).sym, Q) != -1);
                    unsigned int temp_len = k - j;
                    if (if_digit && temp_len == 3) {
                        if (str_stack.at(k - 1).sym == '+')
                            str_stack.at(k).val = str_stack.at(k - 2).val + str_stack.at(k).val;

                        if (str_stack.at(k - 1).sym == '*')
                            str_stack.at(k).val = str_stack.at(k - 2).val * str_stack.at(k).val;

                        if (str_stack.at(k).sym == ')')
                            str_stack.at(k).val = str_stack.at(k - 1).val;
                    }
                    // 此处如果不使用规范规约会产生问题
                    str_stack.erase(str_stack.begin() + j + 1, str_stack.begin() + k);
                    k = j + 1;
                    str_stack.at(k).sym = 'N';
                }
            } catch (const char *ch) {
                cout << ch << endl;
                break;
            }
            try {
                if (priority_judgement(str_stack.at(j).sym, SYM.sym) != 1) {
                    k++;
                    str_stack.emplace_back(SYM);
                }
            } catch (const char *ch) {
                cout << ch << endl;
                break;
            }
        } while (sym < input.size());
        // 结果输出部分
        if (str_stack.size() == 3 && str_stack.at(0).sym == '#' && str_stack.at(2).sym == '#') {
            cout << "success" << endl;
            if (if_digit)
                cout << str_stack.at(1).val << endl;
        } else
            cout << "failed" << endl;
    }

}