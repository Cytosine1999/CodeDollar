// #define TEST_FILE_DIR R"(E:\Projects\CodeDollar\main.cd)"
#define TEST_FILE_DIR R"(/home/cytosine/Projects/CodeDollar/main.cd)"

#include <iostream>
#include <fstream>

using namespace std;

#include "Compiler/Lexer.h"


int main() {
    ifstream file;
    using charT = ifstream::char_type;
    file.open(TEST_FILE_DIR);
    BasicFile<charT> test{file};
    file.close();

    int count = 0;
    for (const auto &line : test) {
        for (const auto &slice: *line) {
            cout << "number: " << count << ' ' << slice->toString() << endl;
            count ++;
        }
    }

/*    Tokenizer<charT> lexer{file};
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
    }*/

    return 0;
}
