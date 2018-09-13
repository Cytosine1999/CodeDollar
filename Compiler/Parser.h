//
// Created by cytosine on 18-9-7.
//

#ifndef CODEDOLLAR_PARSER_H
#define CODEDOLLAR_PARSER_H


#include <iostream>
#include <vector>
#include <memory>

using namespace std;

#include "Lexer.h"
#include "Lexeme.h"
#include "Node.h"
#include "Compiler.h"


template<typename T>
class Scope {
public:
    Scope() : lambdas{}, buffer{}, symbols{} {}

    void add_parameters(Lexeme<T> lexeme) {
        auto symbol = make_shared<unique_ptr<Lambda<T>>>();
        symbols.push_back(make_pair(lexeme, symbol));
        auto lambda = make_unique<Lambda<T>>(symbol);
        if (lambdas.size() != 0) {
            lambdas.back()->set_root(Root<T>{unique_ptr<Node<T>>{new LambdaNode<T>{*lambda}}});
        }
        lambdas.push_back(move(lambda));
    }

    void colon_init(Lexeme<T> lexeme) {
        buffer.push_back(make_pair(lexeme, Root<T>{}));
    }

    void brace_enter(Lexeme<T> lexeme) {
        buffer.push_back(make_pair(lexeme, Root<T>{}));
    }

    void push(unique_ptr<Node<T>> &&node) {
        buffer.back().second.push(move(node));
    }

    void brace_leave(Lexeme<T> lexeme) {
        if (buffer.back().first.type == Type::LEFT_BRACE) {
            auto tmp = move(buffer.back().second);
            buffer.pop_back();
            buffer.back().second.push(move(tmp));
        } else {
            throw CompilerError("Unexpected right brace.");
        }
    }

    unique_ptr<Lambda<T>> clean_up() {
        if (buffer.size() != 1) {
            throw CompilerError("missing right brackets");
        }
        lambdas.back()->set_root(move(buffer.back().second));
        for (auto i = lambdas.size(); i > 1; i--) {
            auto back = move(lambdas.back());
            lambdas.pop_back();
            lambdas.back()->add_lambda(move(back));
        }
        return move(lambdas.back());
    }

    void lambda_add(Lexeme<T> lexeme) {
        buffer.push_back(make_pair(lexeme, Root<T>{}));
    }

    vector<unique_ptr<Lambda<T>>> lambdas;
    vector<pair<Lexeme<T>, Root<T>>> buffer;
    vector<pair<Lexeme<T>, shared_ptr<unique_ptr<Lambda<T>>>>> symbols;
};


template<typename T>
class Parser {
public:
    Parser(BasicFile<T> &file) :
            file{file}, stack{} {
        stack.push_back(Scope<T>{});
        stack.back().buffer.push_back(make_pair(Lexeme<T>{}, Root<T>{}));
    }

    void parse() {
        try {
            while (file.load()) {
                auto &line = file.back();
                while (line.load()) {
                    Lexeme<T> lexeme = line.back().toLexeme();
                    switch (lexeme.type) {
                        case Type::LEFT_BRACE:
                            stack.back().brace_enter(lexeme);
                            break;
                        case Type::RIGHT_BRACE:
                            stack.back().brace_leave(lexeme);
                            break;
                        case Type::LEFT_BRACKET :
                            if (stack.back().buffer.back().first.type == Type::LAMBDA) {
                                stack.push_back(Scope<T>{});
                            } else {
                                throw CompilerError("unbounded block scope");
                            }
                            break;
                        case Type::RIGHT_BRACKET :
                            right_brackets_resolve(lexeme);
                            break;
                        case Type::COLON:
                            stack.back().colon_init(lexeme);
                            break;
                        case Type::LAMBDA :
                            stack.back().lambda_add(lexeme);
                            break;
                        case Type::IDENTIFICATION:
                            if (stack.back().buffer.size() == 0) {
                                stack.back().add_parameters(lexeme);
                            } else if (stack.back().buffer.back().first.type == Type::LAMBDA) {
                                throw CompilerException("explicit lambda not supported yet");
                            } else {
                                stack.back().push(unique_ptr<Node<T>>{new LambdaLink<T>{find_symbol(lexeme)}});
                            }
                    }
                }
            }
        } catch (CompilerError &e) {
            cout << e.what() << endl;
            throw e;
        }
    }

    void right_brackets_resolve(Lexeme<T> lexeme) {
        auto lambda = stack.back().clean_up();
        stack.pop_back();
        auto lambdaNode = unique_ptr<Node<T>>{new LambdaNode<T>{*lambda}};
        stack.back().buffer.pop_back();
        stack.back().buffer.back().second.push(move(lambdaNode));
        stack.back().lambdas.push_back(move(lambda));
    }

    shared_ptr<unique_ptr<Lambda<T>>> find_symbol(Lexeme<T> lexeme) {
        for (auto begin = stack.rbegin(), end = stack.rend(); begin != end; ++begin) {
            for (auto each: begin->symbols) {
                if (each.first.toString() == lexeme.toString()) {
                    return each.second;
                }
            }
        }
        throw CompilerException("unbounded symbol");
    }

protected:
    BasicFile<T> &file;
    vector<Scope<T>> stack;

};


#endif //CODEDOLLAR_PARSER_H
