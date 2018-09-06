//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_H
#define CODEDOLLAR_LEXER_H


#include <istream>
#include <string>
#include <regex>
#include <vector>
#include <iterator>

using namespace std;

#include "Compiler.h"
#include "Tree.h"
#include "LexerBase.h"


template<typename T>
class CodeLine;

template<typename T>
class FileSliceString;


template<typename T>
class BasicFile : public File<T> {
public:
    typedef basic_string<T> stringT;
    typedef basic_istream<T> istreamT;
    typedef File<T> FileT;
    typedef FileLine<T> FileLineT;
    typedef CodeLine<T> CodeLineT;

    explicit BasicFile(istreamT &content) : FileT{} {
        stringT tmp{};
        while (getline(content, tmp)) {
            this->push_back(new CodeLineT{*this, this->size(), move(tmp)});
        }
    }
};


template<typename T>
class CodeLine : public FileLine<T> {
public:
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef File<T> FileT;
    typedef FileLine<T> FileLineT;
    typedef FileSlice<T> FileSliceT;
    typedef FileSliceString<T> FileSliceStringT;

    CodeLine(FileT &file, size_t lineNum, string line) :
            FileLineT{file, lineNum} {
        stringT tmp{};
        stringT_iter begin = line.begin();
        stringT_iter end = line.end();
        while (FileSliceStringT::match(begin, end, tmp)) {
            begin += tmp.length();
            this->push_back(new FileSliceStringT{*this, move(tmp)});
        }
        if (begin != end) {
            throw CompilerException("Lexer failed.");
        }
    }
};


template<typename T>
class FileSliceString : public FileSlice<T> {
public:
    typedef FileSlice<T> FileSliceT;
    typedef basic_string<T> stringT;
    typedef typename stringT::const_iterator stringT_iter;
    typedef FileLine<T> FileLineT;
    typedef basic_regex <T> regexT;
    typedef match_results<typename stringT::const_iterator> matchesT;

    static bool match(stringT_iter start, stringT_iter end, stringT &match) {
        return regex_match(start, end, match, SPACE) ||
               regex_match(start, end, match, ONE_CHARACTER_SYMBOL) ||
               regex_match(start, end, match, FALL_BACK);
    }

    FileSliceString(FileLineT &line, stringT slice, size_t columnNum = 0) :
            FileSliceT{line, columnNum}, slice{slice} {}

    FileSliceString(const FileSliceString &other) = default;

    FileSliceString &operator=(const FileSliceString &other) = default;

    const stringT &toString() const override final {
        return slice;
    }

    size_t length() const override final {
        return slice.length();
    }

    virtual ~FileSliceString() = default;

protected:
    static const regexT SPACE;
    static const regexT ONE_CHARACTER_SYMBOL;
    static const regexT FALL_BACK;

    static bool regex_match(stringT_iter start, stringT_iter end, stringT &match, regexT pattern) {
        matchesT matches;
        if (regex_search(start, end, matches, pattern)) {
            match = matches[0];
            return true;
        } else {
            return false;
        }
    }

    stringT slice;
};


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

    friend istreamT &operator>>(istreamT &stream, CodeSlice &self) {
        return stream >> self.slice;
    }

    friend ostreamT &operator<<(ostreamT &stream, CodeSlice &self) {
        return stream << self.slice;
    }

    operator stringT() const {
        return slice;
    }

    const stringT &toString() const {
        return slice;
    }

    void setLocation(size_t line, size_t column) {
        this->line = line;
        this->column = column;
    }

    size_t getLine() const {
        return line;
    }

    size_t getColumn() const {
        return column;
    }

    size_t len() const {
        return slice.length();
    }

    CodeSlice subSlice(size_t start) {
        return CodeSlice{slice.substr(start), line, column + start};
    }

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
    typedef basic_regex <T> regexT;
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
    static const regexT ONE_CHARACTER_SYMBOL;
    static const regexT NONE_SYMBOL;

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


#include "Lexer.tcc"

#endif //CODEDOLLAR_LEXER_H
