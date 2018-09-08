// #define TEST_FILE_DIR R"(E:\Projects\CodeDollar\main.cd)"
#define TEST_FILE_DIR R"(/home/cytosine/Projects/CodeDollar/main.cd)"


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
    SyntaxTree tree;
    Parser<charT> parser{test, tree};
    parser.parse();

    file.close();

    return 0;
}
