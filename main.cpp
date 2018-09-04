#include <iostream>
#include <fstream>

using namespace std;

#include "Compiler/Lexer.h"


int main() {
    ifstream file;
    using charT = ifstream::char_type;
    file.open(R"(E:\Projects\CodeDollar\main.cd)");
    Tokenizer<charT> lexer{file};

    CodeSlice<charT> element;
    int element_num = 0;
    while (lexer.next(element)) {
        cout << element
             << " Element: "
             << element_num
             << " Line: "
             << element.getLine()
             << " Column: "
             << element.getColumn()
             << endl;
        element_num++;
    }

    file.close();

    return 0;
}
