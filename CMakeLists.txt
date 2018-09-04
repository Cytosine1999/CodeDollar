cmake_minimum_required(VERSION 3.10)
project(CodeDollar)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os")
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(CodeDollar main.cpp Compiler/Lexer.cpp Compiler/Lexer.h Compiler/Compiler.h)