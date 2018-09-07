// #define TEST_FILE_DIR R"(E:\Projects\CodeDollar\main.cd)"
#define TEST_FILE_DIR R"(/home/cytosine/Projects/CodeDollar/main.cd)"

#include <iostream>
#include <fstream>

using namespace std;

#include "Compiler/Lexer.h"


int main() {
    try {
        ifstream file;
        using charT = ifstream::char_type;
        file.open(TEST_FILE_DIR);
        BasicFile<charT> test{file};
        file.close();

        int count = 0;
        for (const auto &line: test.codeLineIter()) {
            for (const auto &slice: line.lexesIter()) {
                cout << "Element:"
                     << count
                     << ",Line:"
                     << slice.getLineNum()
                     << ",Column:"
                     << slice.getColumnNum()
                     << endl
                     << slice.toString()
                     << endl;
                count++;
            }
        }
    } catch (CompilerError &e) {
        ;
    }

    return 0;
}
