#define TEST_FILE_DIR R"(E:\Projects\CodeDollar\main.cd)"             // for windows
// #define TEST_FILE_DIR R"(/home/cytosine/Projects/CodeDollar/main.cd)"             // for linux
// #define TEST_FILE_DIR R"(/mnt/e/Projects/CodeDollar/main.cd)"       // for WSL

#include <iostream>
#include <fstream>

using namespace std;

#include "Compiler/Lexer.h"
#include "Compiler/Parser.h"


int main() {
    ifstream file;
    using charT = ifstream::char_type;
    file.open(TEST_FILE_DIR);
    BasicFile<charT> test{file};
    Parser<charT> parser{test};
    auto main = parser.parse();

    auto ret = main.calculate();

    cout << ret.toString() << endl;

    file.close();

    return 0;
}
