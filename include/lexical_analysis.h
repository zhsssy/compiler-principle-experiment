//
// Created by zhsssy.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_LEXICAL_ANALYSIS_H
#define COMPILER_PRINCIPLE_EXPERIMENT_LEXICAL_ANALYSIS_H

#include <string>
#include <fstream>
#include <vector>
#include <set>

using namespace std;


enum LEXICAL_RESULT {
    ERROR = -1,
    IDENTIFICATION = 0,
    DIGIT,
    OCT,
    HEX,
    KEY,
    END
};


class LexicalAnalysis {
private:
    ifstream fin;
    std::string tmp;
    std::string token;
    const char *p;
    LEXICAL_RESULT result;
    std::set<std::string> keyword = {"do", "while", "if", "else", "then"};

public:
    explicit LexicalAnalysis(const std::string &filename)
            : fin(filename, ios_base::in), p(nullptr), result(ERROR) {
    }

    bool scan() {
        skip_blank();

        if (p == nullptr || *p == 0) {
            result = END;
            token = "#";
            return false;
        }

        token_init();

        if (isalpha(*p)) {
            add();
            while (isalnum(*p))
                add();

            if (iskeyword()) result = KEY;
            else result = IDENTIFICATION;
        } else if (*p == '0') {
            add();
            if (*p == 'x' || *p == 'X') {
                add();
                if (is_hex(*p)) {
                    add();
                    while (is_hex(*p))
                        add();
                    result = HEX;
                } else {
                    add();
                    result = ERROR;
                }
            } else if (isdigit(*p)) {
                if (*p != '0' && *p != '8' && *p != '9') {
                    add();
                    while (is_oct(*p))
                        add();
                    result = OCT;
                } else {
                    add();
                    result = ERROR;
                }
            } else result = DIGIT;
        } else if (isdigit(*p)) {
            add();
            while (isdigit(*p))
                add();
            result = DIGIT;
        } else add(), result = KEY;

        return true;
    }

    inline LEXICAL_RESULT get_result() const {
        return result;
    }

    inline std::string get_token() const {
        return token;
    }


    bool is_open() const {
        return fin.is_open();
    }

    static inline bool is_input(LEXICAL_RESULT r) {
        return r == LEXICAL_RESULT::HEX
               || r == LEXICAL_RESULT::OCT
               || r == LEXICAL_RESULT::DIGIT
               || r == LEXICAL_RESULT::IDENTIFICATION;
    }

    static inline bool is_key(LEXICAL_RESULT r) {
        return r == LEXICAL_RESULT::KEY;
    }

private:
    void skip_blank() {
        if (p != nullptr)
            while (*p < 33 && *p > 0) ++p;

        while ((p == nullptr || *p == 0) && getline(fin, tmp)) {
            if (tmp.empty()) continue;
            tmp.erase(0, tmp.find_first_not_of(' '));
            if (tmp.empty()) continue;
            tmp.erase(tmp.find_last_not_of(' ') + 1);

            p = tmp.c_str();

            while (*p < 33 && *p > 0) ++p;
        }
    }

    void token_init() {
        token.erase(token.cbegin(), token.cend());
    }

    inline void add() {
        token += *p++;
    }

    inline bool iskeyword() const {
        return keyword.contains(get_token());
    }

    static inline bool is_oct(const char ch) {
        return ch >= '0' && ch < '8';
    }

    static inline bool is_hex(const char ch) {
        return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f');
    }
};


#endif //COMPILER_PRINCIPLE_EXPERIMENT_LEXICAL_ANALYSIS_H
