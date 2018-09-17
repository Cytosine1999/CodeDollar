//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_H
#define CODEDOLLAR_LEXER_H


#include <iostream>
#include <istream>
#include <string>
#include <regex>
#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;

#include "Compiler.h"
#include "Tree.h"
#include "LexerBase.h"
#include "Lexeme.h"


template<typename T>
class CodeLine;


// TODO the /r/n issue not handled
template<typename T>
class BasicFile : public File<T> {
public:
    typedef basic_string<T> stringT;
    typedef basic_istream<T> istreamT;
    typedef File<T> FileT;
    typedef CodeLine<T> CodeLineT;
    typedef vector<const CodeLineT *> vectorT;

    explicit BasicFile(istreamT &content) : FileT{}, content{content} {}

    BasicFile(const BasicFile &other) = default;

    BasicFile &operator=(const BasicFile &other) = default;

    bool load() final {
        stringT tmp{};
        if (getline(content, tmp)) {
            auto ptr = new CodeLineT{this, this->size(), move(tmp)};
            this->push_back(ptr);
            return true;
        } else {
            return false;
        }
    }

    virtual ~BasicFile() = default;

protected:
    istreamT &content;
};

template<typename T>
class CodeLine : public FileLine<T> {
public:
    typedef basic_string<T> stringT;
    typedef BasicFile<T> BasicFileT;

    CodeLine(BasicFileT *file, size_t lineNum, stringT line) : FileLine<T>{file, lineNum, line}, offset{0} {}

    CodeLine(const CodeLine &other) = default;

    CodeLine &operator=(const CodeLine &other) = default;

    bool load() final;

    virtual ~CodeLine() = default;

protected:
    size_t offset;

};


template<typename T>
class CodeSpace : public FileSlice<T> {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef basic_regex <T> regexT;
    typedef CodeLine<T> CodeLineT;
    typedef FileSlice<T> FileSliceT;

    static bool match(stringT_iter start, stringT_iter end, stringT &match) {
        return FileSliceT::REGEX_MATCH(start, end, match, SPACE);
    }

    CodeSpace(CodeLineT *line, size_t columnNum, stringT slice) :
            FileSliceT{line, columnNum, slice} {}

    CodeSpace(const CodeSpace &other) = default;

    CodeSpace &operator=(const CodeSpace &other) = default;

    virtual ~CodeSpace() = default;

protected:
    static const regexT SPACE;
};

template<typename T>
const basic_regex<T> CodeSpace<T>::SPACE{R"(^\s+)"};



template<typename T>
class CodeSymbol : public FileSlice<T> {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef basic_regex <T> regexT;
    typedef CodeLine<T> CodeLineT;
    typedef FileSlice<T> FileSliceT;

    static bool match(stringT_iter start, stringT_iter end, stringT &match) {
        return FileSliceT::REGEX_MATCH(start, end, match, ONE_CHARACTER_SYMBOL);
    }

    CodeSymbol(CodeLineT *line, size_t columnNum, stringT slice) :
            FileSliceT{line, columnNum, slice} {}

    CodeSymbol(const CodeSymbol &other) = default;

    CodeSymbol &operator=(const CodeSymbol &other) = default;

    virtual Lexeme<T> toLexeme() const final {
        return SymbolLexeme<T>{*this};
    };

    virtual ~CodeSymbol() = default;

protected:
    static const regexT ONE_CHARACTER_SYMBOL;
};

template<typename T>
const basic_regex<T> CodeSymbol<T>::ONE_CHARACTER_SYMBOL{R"(^[\$\:\(\)\{\}])"};



template<typename T>
class CodeName : public FileSlice<T> {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef basic_regex <T> regexT;
    typedef CodeLine<T> CodeLineT;
    typedef FileSlice<T> FileSliceT;

    static bool match(stringT_iter start, stringT_iter end, stringT &match) {
        return FileSliceT::REGEX_MATCH(start, end, match, NAME);
    }

    CodeName(CodeLineT *line, size_t columnNum, stringT slice) :
            FileSliceT{line, columnNum, slice} {}

    CodeName(const CodeName &other) = default;

    CodeName &operator=(const CodeName &other) = default;

    virtual Lexeme<T> toLexeme() const final {
        return NameLexeme<T>{*this};
    };

    virtual ~CodeName() = default;

protected:
    static const regexT NAME;
};

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


#endif //CODEDOLLAR_LEXER_H
