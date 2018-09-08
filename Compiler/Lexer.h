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

    virtual Lexeme toLexeme() const final {
        return SymbolLexeme{};
    };

    virtual ~CodeSymbol() = default;

protected:
    static const regexT ONE_CHARACTER_SYMBOL;
};


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

    virtual Lexeme toLexeme() const final {
        return NameLexeme{};
    };

    virtual ~CodeName() = default;

protected:
    static const regexT NAME;
};


#include "Lexer.tcc"

#endif //CODEDOLLAR_LEXER_H
