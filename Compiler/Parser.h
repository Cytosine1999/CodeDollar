//
// Created by cytosine on 18-9-7.
//

#ifndef CODEDOLLAR_PARSER_H
#define CODEDOLLAR_PARSER_H


#include <iostream>

using namespace std;

#include "Lexer.h"


class SyntaxTree {
public:
    template <typename T>
    void push(FileSlice<T> &slice) {
        cout << "Line:"
             << slice.getLineNum()
             << ",Column:"
             << slice.getColumnNum()
             << endl
             << slice.toString()
             << endl;
    };

protected:

};


template <typename T>
class Parser {
public:
    Parser(BasicFile<T> &file, SyntaxTree &tree) : file{file}, tree{tree} {}

    void parse() {
        try {
            while (file.load()) {
                auto &line = file.back();
                while (line.load()) {
                    tree.push(line.back());
                }
            }
        } catch (CompilerError &e) { ;
        }
    }

protected:
    BasicFile<T> &file;
    SyntaxTree &tree;
};




#endif //CODEDOLLAR_PARSER_H
