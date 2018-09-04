//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_H
#define CODEDOLLAR_LEXER_H


#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <regex>

using namespace std;

#include "Compiler.h"


template<typename T>
class CodeSlice {
public:
    typedef basic_string<T> stringT;
    typedef basic_istream<T> istreamT;
    typedef basic_ostream<T> ostreamT;

    CodeSlice() : CodeSlice{string{}} {};

    explicit CodeSlice(const stringT &other) : slice{other}, line{0}, column{0} {}

    CodeSlice(stringT slice, size_t line, size_t column) : slice{slice}, line{line}, column{column} {}

    CodeSlice(const CodeSlice &other) = default;

    CodeSlice &operator=(const CodeSlice &other) = default;

    friend istreamT &operator>>(istreamT &stream, CodeSlice &self) { return stream >> self.slice; }

    friend ostreamT &operator<<(ostreamT &stream, CodeSlice &self) { return stream << self.slice; }

    operator stringT() const { return slice; }

    const stringT &toString() const { return slice; }

    void setLocation(size_t line, size_t column) { this->line = line, this->column = column; }

    size_t getLine() const { return line; }

    size_t getColumn() const { return column; }

    size_t len() const { return slice.length(); }

    CodeSlice subSlice(size_t start) { return CodeSlice{slice.substr(start), line, column + start}; }

    ~CodeSlice() = default;

private:
    stringT slice;
    size_t line, column;
};


template<typename T>
class Spliter {
public:
    typedef basic_istream<T> istreamT;
    typedef basic_istringstream<T> isstreamT;
    typedef CodeSlice<T> CodeSliceT;

    explicit Spliter(istreamT &input) : input{input}, line{}, lineNum{0}, lineLen{0} {}

    Spliter(const Spliter &other) = delete;

    Spliter(Spliter &&other) noexcept = default;

    Spliter &operator=(const Spliter &other) = delete;

    Spliter &operator=(Spliter &&other) noexcept = default;

    bool next(CodeSliceT &element) {
        if (lineNext(element)) {
            return true;
        } else {
            string tmp;
            if (getline(input, tmp)) {
                line = isstreamT{move(tmp)};
                lineLen = tmp.length();
                lineNum++;
                return lineNext(element);
            } else {
                return false;
            }
        }
    }

    ~Spliter() = default;

private:
    istreamT &input;
    isstreamT line;
    size_t lineNum, lineLen;

    bool lineNext(CodeSliceT &element) {
        if (line >> element) {
            size_t column = (line.tellg() > 0 ? (size_t) line.tellg() : lineLen) - element.len() + 1;
            element.setLocation(lineNum, column);
            return true;
        } else {
            return false;
        }
    }
};


template<typename T>
class Tokenizer {
public:
    typedef basic_string<T> stringT;
    typedef Spliter<T> SpliterT;
    typedef basic_regex<T> regexT;
    typedef typename SpliterT::istreamT istreamT;
    typedef match_results<typename stringT::const_iterator> resultsT;
    typedef CodeSlice<T> CodeSliceT;

    explicit Tokenizer(istreamT &input) : spliter{input}, buffer{} {}

    Tokenizer(const Tokenizer &other) = delete;

    Tokenizer(Tokenizer &&other) noexcept = default;

    Tokenizer &operator=(const Tokenizer &other) = delete;

    Tokenizer &operator=(Tokenizer &&other) noexcept = default;

    bool next(CodeSliceT &matches) {
        if (buffer.len() == 0 && !spliter.next(buffer)) return false;
        if (token(matches, ONE_CHARACTER_SYMBOL)) return true;
        if (token(matches, NONE_SYMBOL)) return true;
        throw CompilerException("Invalid character.");
    }

    ~Tokenizer() = default;

private:
    const regexT ONE_CHARACTER_SYMBOL = regex{R"(^[\$\:\(\)\{\}])"};
    const regexT NONE_SYMBOL = regex{R"(^((?![\$\:\(\)\{\}])\S)+)"};

    SpliterT spliter;
    CodeSliceT buffer;

    bool token(CodeSliceT &matches, regexT pattern) {
        resultsT regex_matches;
        if (regex_search(buffer.toString(), regex_matches, pattern)) {
            matches = CodeSliceT{regex_matches[0], buffer.getLine(), buffer.getColumn()};
            buffer = buffer.subSlice(matches.len());
            return true;
        } else {
            return false;
        }
    }
};


#endif //CODEDOLLAR_LEXER_H
