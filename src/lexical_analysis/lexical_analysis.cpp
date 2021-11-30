//
// Created by zhsssy.
//

#include <lexical_analysis.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    string token, s;
    LEXICAL_RESULT result;

    if (argc < 2) {
        cout << "open file error: check path or filename" << endl;
        exit(EXIT_FAILURE);
    }

    LexicalAnalysis l(argv[1]);

    if (!l.is_open()) {
        cout << "Can't open file." << endl;
        exit(EXIT_FAILURE);
    }

    while (l.scan()) {
        result = l.get_result();
        switch (result) {
            case IDENTIFICATION:
            case DIGIT:
            case OCT:
            case HEX:
                cout << "<" << result << ", " << l.get_token() << ">" << endl;
                break;
            case KEY:
                cout << "<" << l.get_token() << ", " << "->" << endl;
                break;
            case ERROR:
                cout << "ERROR" << endl;
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }
}


