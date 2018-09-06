cmake_minimum_required(VERSION 3.10)
project(CodeDollar)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Os")
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(CodeDollar main.cpp Compiler/Lexer.tcc Compiler/Lexer.h Compiler/Compiler.h Compiler/Tree.h Compiler/LexerBase.h)