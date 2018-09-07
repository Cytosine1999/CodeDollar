//
// Created by cytosine on 18-9-6.
//

#ifndef CODEDOLLAR_LEXERBASE_H
#define CODEDOLLAR_LEXERBASE_H


#include <istream>
#include <string>
#include <regex>
#include <vector>
#include <iterator>

using namespace std;

#include "Compiler.h"
#include "Tree.h"

using namespace Compiler;


template<typename T>
class FileLine;

template<typename T>
class FileSlice;


template<typename T>
class File : public Tree<FileLine<T>> {
public:
    typedef basic_string<T> stringT;
    typedef Tree<FileLine<T>> TreeT;
    typedef FileLine<T> FileLineT;
    typedef FileSlice<T> FileSliceT;

    File() : TreeT{}, len{0} {}

    File(const File &other) = default;

    File &operator=(const File &other) = default;

    size_t length() const {
        return len;
    }

    stringT toString() const{
        stringT tmp{};
        tmp.reserve(this->length());
        for (const auto &line: *this) {
            tmp += line->toString() + '\n';
        }
        return tmp;
    }

    virtual ~File() = default;

protected:
    size_t len;
};


template<typename T>
class FileLine : public Tree<FileSlice<T>> {
public:
    typedef basic_string<T> stringT;
    typedef Tree<FileSlice<T>> TreeT;
    typedef File<T> FileT;
    typedef FileSlice<T> FileSliceT;

    FileLine(const FileT *file, size_t lineNum, size_t length) :
            TreeT{}, file{file}, lineNum{lineNum}, len{length} {}

    FileLine(const FileLine &other) = default;

    FileLine &operator=(const FileLine &other) = default;

    size_t length() const {
        return len;
    }

    size_t getLineNum() const {
        return lineNum;
    }

    stringT toString() const {
        stringT tmp{};
        tmp.reserve(this->length());
        for (const auto &slice: *this) {
            tmp += slice->toString();
        }
        return tmp;
    };

    virtual ~FileLine() = default;

protected:
    const FileT *file;
    size_t lineNum;
    size_t len;
};


template<typename T>
class FileSlice {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef basic_regex<T> regexT;
    typedef FileLine<T> FileLineT;
    typedef match_results<stringT_iter> resultsT;

    FileSlice(const FileLineT *line, size_t columnNum, size_t sliceNum, stringT slice) :
            line{line}, columnNum{columnNum}, sliceNum{sliceNum}, slice{slice} {}

    FileSlice(const FileSlice &other) = default;

    FileSlice &operator=(const FileSlice &other) = default;

    size_t getLineNum() const {
        return line->getLineNum();
    }

    size_t getColumnNum() const {
        return columnNum;
    }

    const stringT &toString() const {
        return slice;
    }

    size_t length() const {
        return slice.length();
    }

    virtual ~FileSlice() = default;

protected:
    static bool REGEX_MATCH(stringT_iter start, stringT_iter end, stringT &match, regexT pattern) {
        resultsT matches;
        if (regex_search(start, end, matches, pattern)) {
            match = matches[0];
            start += match.length();
            return true;
        } else {
            return false;
        }
    }

    const FileLineT *line;
    size_t columnNum;
    size_t sliceNum;
    stringT slice;
};


#endif //CODEDOLLAR_LEXERBASE_H
