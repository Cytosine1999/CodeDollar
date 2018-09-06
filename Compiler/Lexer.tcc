//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_LEXER_TCC
#define CODEDOLLAR_LEXER_TCC


template<typename T>
const basic_regex<T> FileSliceString<T>::SPACE{R"(^\s+)"};

template<typename T>
const basic_regex<T> FileSliceString<T>::ONE_CHARACTER_SYMBOL{R"(^[\$\:\(\)\{\}])"};

template<typename T>
const basic_regex<T> FileSliceString<T>::FALL_BACK{R"(^((?![\$\:\(\)\{\}])\S)+)"};

template<typename T>
const basic_regex<T> Tokenizer<T>::ONE_CHARACTER_SYMBOL{R"(^[\$\:\(\)\{\}])"};

template<typename T>
const basic_regex<T> Tokenizer<T>::NONE_SYMBOL{R"(^((?![\$\:\(\)\{\}])\S)+)"};


#endif //CODEDOLLAR_LEXER_TCC
