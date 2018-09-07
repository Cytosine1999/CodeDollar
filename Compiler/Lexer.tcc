//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_TCC
#define CODEDOLLAR_LEXER_TCC


#include "sstream"


template<typename T>
const basic_regex<T> CodeSpace<T>::SPACE{R"(^\s+)"};

template<typename T>
const basic_regex<T> CodeLexeme<T>::ONE_CHARACTER_SYMBOL{R"(^[\$\:\(\)\{\}])"};

template<typename T>
const basic_regex<T> CodeLexeme<T>::NAME{R"(^[_[:alpha:]]\w*)"};

template<typename T>
CodeLine<T>::CodeLine(BasicFile<T> *file, size_t lineNum, basic_string<T> line) :
        FileLine<T>{file, lineNum, line.length()}, lexis{} {
    auto begin = line.begin();
    auto end = line.end();
    size_t column = 0;
    basic_string<T> tmp{};
    while (begin != end) {
        if (CodeSpace<T>::match(begin, end, tmp)) {
            auto ptr = new CodeSpace<T>{this, column, this->size(), move(tmp)};
            column += ptr->length();
            begin += ptr->length();
            this->push_back(ptr);
        } else if (CodeLexeme<T>::match(begin, end, tmp)) {
            auto ptr = new CodeLexeme<T>{this, column, this->size(), move(tmp)};
            column += ptr->length();
            begin += ptr->length();
            this->push_back(ptr);
            lexis.push_back(ptr);
        } else {
            cout << "Invalid character in line " << lineNum + 1 << ", column " << column + 1 << ":\n"
                 << basic_string<T>(line.length(), '-') << '\n'
                 << line << '\n'
                 << basic_string<T>(column, ' ') << "^\n"
                 << basic_string<T>(line.length(), '-') << '\n';
            throw CompilerError("Invalid character!");
        }
    }
}


#endif //CODEDOLLAR_LEXER_TCC
