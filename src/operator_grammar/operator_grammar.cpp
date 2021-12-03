//
// Created by zhsssy.
//

#include "operator_grammar.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s \"input filename\" \"output filename\"\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 构造 需要用到辅助函数
    grammar g = create_grammar_by_file(argv[1]);
    LexicalAnalysis l(argv[2]);
    g.print_production();
    for (auto i: g.get_vt()) {
        cout << i << " ";
    }
    cout << endl;

    unsigned k, j;
    char N;
    string token_str;
    bool judge = true;
    size_t sym;
    char SYM, Q;
    string str_stack;

    if (!l.is_open()) {
        cerr << "Input filename isn't exist!" << endl;
        exit(EXIT_FAILURE);
    }

    // 根据 ; 全部分割再判断
    vector<string> inputs;
    string temp_inputs;
    while (l.scan()) {
        if (l.get_token().at(0) == ';') {
            temp_inputs += "#";
            inputs.push_back(temp_inputs);
            temp_inputs = "";
        }
        if (LexicalAnalysis::is_input(l.get_result())) {
            temp_inputs += 'i';
        } else if (LexicalAnalysis::is_key(l.get_result()) && l.get_token() != ";") {
            temp_inputs += l.get_token().at(0);
        }
    }

    for (const auto &input: inputs) {
        k = 0;
        str_stack = "#";
        sym = 0;
        do {
            SYM = input.at(sym++);
            if (str_stack.at(k) == '#' || g.is_vt(str_stack.at(k))) j = k;
            else j = k - 1;
//            cout << "str len: " << str_stack.length() << " " << str_stack << endl;
            try {
                while (priority_judgement(str_stack.at(j), SYM) == 1) {
                    do {
                        Q = str_stack.at(j);
                        if (j <= 0) break;
                        if (j > 0 && (str_stack.at(j - 1) == '#' || g.is_vt(str_stack.at(j - 1)))) j = j - 1;
                        else j = j - 2;
                    } while (priority_judgement(str_stack.at(j), Q) == -1);
                    N = g.fuzzy_reduced(str_stack.substr(j + 1, k));
                    str_stack.erase(j + 1, k - j - 1);
                    k = j + 1;
                    str_stack += N;
//                    cout << "str2 len: " << str_stack.length() << " " << str_stack << endl;
                }
            }
            catch (ReduceException e) {
                cout << "reduce error" << endl;
            }
            catch (const char *ch) {
                cout << input << ch << endl;
            }
            try {
                if (priority_judgement(str_stack.at(j), SYM) != 1) {
                    k++;
                    str_stack += SYM;
                } else {
                    judge = false;
                    break;
                }
            } catch (const char *ch) {
                cout << input << ch << endl;
            }
        } while (input[sym] != '#' && sym < input.size());
        cout << input << (judge ? "success" : "failed") << endl;
    }

}