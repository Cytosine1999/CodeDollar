//
// Created by cytosine on 18-9-8.
//

#ifndef CODEDOLLAR_LEXEME_H
#define CODEDOLLAR_LEXEME_H


#include <string>

using namespace std;


enum class Type : char {
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COLON,
    LAMBDA,
    IDENTIFICATION,
};


template <typename T>
class FileSlice;


template <typename T>
class Lexeme {
public:
    Lexeme() : slice{nullptr}, type{Type::COLON} {}

    Lexeme(const FileSlice<T> &slice) : slice{&slice}, type{Type::COLON} {}

    const string &toString() {
        return slice->toString();
    }

    const FileSlice<T> *slice;
    Type type;
};


template <typename T>
class SymbolLexeme : public Lexeme<T> {
public:
    explicit SymbolLexeme(const FileSlice<T> &slice) : Lexeme<T>{slice} {
        if (slice.toString() == "(") {
            this->type = Type::LEFT_BRACE;
        } else if (slice.toString() == ")") {
            this->type = Type::RIGHT_BRACE;
        } else if (slice.toString() == "{") {
            this->type = Type::LEFT_BRACKET;
        } else if (slice.toString() == "}") {
            this->type = Type::RIGHT_BRACKET;
        } else if (slice.toString() == ":") {
            this->type = Type::COLON;
        } else if (slice.toString() == "$") {
            this->type = Type::LAMBDA;
        }
    }
};


template <typename T>
class NameLexeme : public Lexeme<T> {
public:
    explicit NameLexeme(const FileSlice<T> &slice) : Lexeme<T>{slice} {
        this->type = Type::IDENTIFICATION;
    }
};


#endif //CODEDOLLAR_LEXEME_H
