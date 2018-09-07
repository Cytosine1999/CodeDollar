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


template<typename T>
class CodeLine;

template<typename T>
class CodeSpace;

template<typename T>
class CodeLexeme;

// TODO the /r/n issue not handled
template<typename T>
class BasicFile : public File<T> {
public:
    typedef basic_string<T> stringT;
    typedef basic_istream<T> istreamT;
    typedef File<T> FileT;
    typedef CodeLine<T> CodeLineT;
    typedef vector<const CodeLineT *> vectorT;

    explicit BasicFile(istreamT &content) : FileT{}, codeLines{} {
        stringT tmp{};
        size_t length = 0;
        while (getline(content, tmp)) {
            auto ptr = new CodeLineT{this, this->size(), move(tmp)};
            this->push_back(ptr);
            codeLines.push_back(ptr);
            length += tmp.length();
        }
        this->len = length + this->length();
    }

    BasicFile(const BasicFile &other) = default;

    BasicFile &operator=(const BasicFile &other) = default;

    RefVecIterable<CodeLineT> codeLineIter() const {
        return RefVecIterable<CodeLineT>{codeLines};
    }

    virtual ~BasicFile() = default;

protected:
    vectorT codeLines;
};

template<typename T>
class CodeLine : public FileLine<T> {
public:
    typedef basic_string<T> stringT;
    typedef BasicFile<T> BasicFileT;
    typedef CodeLexeme<T> LexisT;
    typedef vector<const LexisT *> vectorT;

    CodeLine(BasicFileT *file, size_t lineNum, stringT line);

    CodeLine(const CodeLine &other) = default;

    CodeLine &operator=(const CodeLine &other) = default;

    RefVecIterable<LexisT> lexesIter() const {
        return RefVecIterable<LexisT>{lexis};
    };

    virtual ~CodeLine() = default;

protected:
    vectorT lexis;

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

    CodeSpace(CodeLineT *line, size_t columnNum, size_t sliceNum, stringT slice) :
            FileSliceT{line, columnNum, sliceNum, slice} {}

    CodeSpace(const CodeSpace &other) = default;

    CodeSpace &operator=(const CodeSpace &other) = default;

    virtual ~CodeSpace() = default;

protected:
    static const regexT SPACE;
};


template<typename T>
class CodeLexeme : public FileSlice<T> {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef basic_regex <T> regexT;
    typedef CodeLine<T> CodeLineT;
    typedef FileSlice<T> FileSliceT;

    static bool match(stringT_iter start, stringT_iter end, stringT &match) {
        return FileSliceT::REGEX_MATCH(start, end, match, ONE_CHARACTER_SYMBOL) ||
               FileSliceT::REGEX_MATCH(start, end, match, NAME);
    }

    CodeLexeme(CodeLineT *line, size_t columnNum, size_t sliceNum, stringT slice) :
            FileSliceT{line, columnNum, sliceNum, slice} {}

    CodeLexeme(const CodeLexeme &other) = default;

    CodeLexeme &operator=(const CodeLexeme &other) = default;

    virtual ~CodeLexeme() = default;

protected:
    static const regexT ONE_CHARACTER_SYMBOL, NAME;
};


#include "Lexer.tcc"

#endif //CODEDOLLAR_LEXER_H
