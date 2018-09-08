//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_TCC
#define CODEDOLLAR_LEXER_TCC


#include "sstream"


template<typename T>
const basic_regex<T> CodeSpace<T>::SPACE{R"(^\s+)"};

template<typename T>
const basic_regex<T> CodeSymbol<T>::ONE_CHARACTER_SYMBOL{R"(^[\$\:\(\)\{\}])"};

template<typename T>
const basic_regex<T> CodeName<T>::NAME{R"(^[_[:alpha:]]\w*)"};


template<typename T>
bool CodeLine<T>::load() {
    auto begin = this->line.begin();
    auto end = this->line.end();
    basic_string<T> tmp{};
    if (CodeSpace<T>::match(begin + offset, end, tmp)) {
        offset += tmp.length();
    }
    if (CodeSymbol<T>::match(begin + offset, end, tmp)) {
        offset += tmp.length();
        auto ptr = new CodeSymbol<T>{this, offset, move(tmp)};
        this->push_back(ptr);
        return true;
    }
    if (CodeName<T>::match(begin + offset, end, tmp)) {
        offset += tmp.length();
        auto ptr = new CodeName<T>{this, offset, move(tmp)};
        this->push_back(ptr);
        return true;
    }
    if (begin + offset != end) {
        cout << "Invalid character at line " << this->lineNum + 1 << ", column " << offset + 1 << ":\n"
             << basic_string<T>(this->line.length(), '-') << '\n'
             << this->line << '\n'
             << basic_string<T>(offset, ' ') << "^\n"
             << basic_string<T>(this->line.length(), '-') << '\n';
        throw CompilerError("Invalid character!");
    } else {
        return false;
    }
}


#endif //CODEDOLLAR_LEXER_TCC
