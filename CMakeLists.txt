cmake_minimum_required(VERSION 3.10)
project(CodeDollar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(CodeDollar main.cpp Compiler/Lexer.h Compiler/Compiler.h Compiler/Tree.h
        Compiler/LexerBase.h Compiler/Parser.h Compiler/Lexeme.h Compiler/Node.h)