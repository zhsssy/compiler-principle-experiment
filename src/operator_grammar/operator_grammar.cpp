//
// Created by zhsssy.
//

#include "operator_grammar.h"

#include <utility>

// S SDT
class Node {
public:
    char sym;
    int val;
    bool hasVal;

    Node() = default;

    Node(char sym) : sym(sym), hasVal(false) {}

    Node(char sym, int val) : sym(sym), val(val), hasVal(true) {}

};


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 构造 需要用到辅助函数
    grammar g = create_grammar_by_file(argv[1]);
    LexicalAnalysis l(argv[2]);
    g.print_production();
    cout << "VT: ";
    for (auto i: g.get_vt()) {
        cout << i << " ";
    }
    cout << endl;

    unsigned k, j;
    char N;
    size_t sym;
    char Q;
    Node SYM;
    vector<Node> str_stack;

    if (!l.is_open()) {
        cerr << "Input filename isn't exist!" << endl;
        exit(EXIT_FAILURE);
    }

    // 根据 ; 全部分割再判断
    vector<vector<Node>> inputs;
    vector<Node> nodes;
    string temp_inputs;
    while (l.scan()) {
        if (l.get_token().at(0) == ';') {
            nodes.emplace_back(Node('#', 0));
            inputs.push_back(nodes);
            nodes.clear();
        }
        if (LexicalAnalysis::is_input(l.get_result())) {
            // 值转换 使用 Node 构造栈
            nodes.emplace_back(Node('i', stoi(l.get_token())));
        } else if (LexicalAnalysis::is_key(l.get_result()) && l.get_token() != ";") {
            nodes.emplace_back(l.get_token().at(0));
        }
    }

    for (const auto &input: inputs) {
        k = 0;
        str_stack.clear();
        str_stack.emplace_back(Node('#', 0));
        sym = 0;
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
                    int temp_i = k - j;
                    //                    N = g.fuzzy_reduced(str_stack.substr(j + 1, k));
                    if (temp_i == 3 && str_stack.at(k - 1).sym == '+')
                        str_stack.at(k).val = str_stack.at(k - 2).val + str_stack.at(k).val;

                    if (temp_i == 3 && str_stack.at(k - 1).sym == '*')
                        str_stack.at(k).val = str_stack.at(k - 2).val * str_stack.at(k).val;

                    if (temp_i == 3 && str_stack.at(k).sym == ')')
                        str_stack.at(k).val = str_stack.at(k - 1).val;

                    str_stack.erase(str_stack.begin() + j + 1, str_stack.begin() + k);
                    k = j + 1;
                    str_stack.at(k).sym = 'N';
                    cout << "k is" << k << " k sym is " << str_stack.at(k).sym << endl;
//                    str_stack.emplace_back(Node('N'));
#ifdef DEBUG
                    cout << "str2 len: " << str_stack.length() << " " << str_stack << endl;
                    cout << "input: " << input.substr(sym, input.size() - sym) << endl;
#endif
                }
            }
            catch (ReduceException e) {
                cout << "reduce error" << endl;
            }
            catch (const char *ch) {
//                cout << input << ch << endl;
            }
            try {
                if (priority_judgement(str_stack.at(j).sym, SYM.sym) != 1) {
                    k++;
                    str_stack.emplace_back(SYM);
                }
            } catch (const char *ch) {
//                cout << input << ch << endl;
            }
        } while (sym < input.size());
        cout << str_stack.at(1).sym << " " << str_stack.at(1).val << endl;
    }

}