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

    File() : TreeT{} {}

    File(const File &other) = default;

    File &operator=(const File &other) = default;

    virtual ~File() = default;
};


template<typename T>
class FileLine : public Tree<FileSlice<T>> {
public:
    typedef basic_string<T> stringT;
    typedef Tree<FileSlice<T>> TreeT;
    typedef File<T> FileT;
    typedef FileSlice<T> FileSliceT;

    FileLine(FileT &file, size_t lineNum) :
            TreeT{}, file{&file}, lineNum{lineNum} {}

    FileLine(const FileLine &other) = default;

    FileLine &operator=(const FileLine &other) = default;

    size_t getLineNum() const {
        return lineNum;
    }

    virtual ~FileLine() = default;

protected:
    FileT *file;
    size_t lineNum;
};


template<typename T>
class FileSlice {
public:
    typedef basic_string<T> stringT;
    typedef FileLine<T> FileLineT;

    FileSlice(FileLineT &line, size_t columnNum = 0) :
            line{&line}, columnNum{columnNum} {}

    FileSlice(const FileSlice &other) = default;

    FileSlice &operator=(const FileSlice &other) = default;

    size_t getLineNum() const {
        return line.getLineNum();
    }

    size_t getColumnNum() const {
        return columnNum;
    }

    virtual const stringT &toString() const = 0;

    virtual size_t length() const = 0;

    virtual ~FileSlice() = default;

protected:
    FileLineT *line;
    size_t columnNum;
};



#endif //CODEDOLLAR_LEXERBASE_H
