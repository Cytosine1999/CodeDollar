//
// Created by cytosine on 18-9-8.
//

#ifndef CODEDOLLAR_LEXEME_H
#define CODEDOLLAR_LEXEME_H


template <typename T>
class FileSlice;


template <typename T>
class Lexeme {
public:
    Lexeme(FileSlice<T> &slice) : slice{slice} {}

protected:
    FileSlice<T> &slice;

};


template <typename T>
class SymbolLexeme : public Lexeme<T> {

};


template <typename T>
class NameLexeme : public Lexeme<T> {

};


#endif //CODEDOLLAR_LEXEME_H
