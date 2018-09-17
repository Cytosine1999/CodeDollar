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
class FallBackNameGen {
public:
    static FallBackNameGen GEN;

    basic_string<T> next() {
        basic_string<T> name = "$" + to_string(counter);
        counter++;
        return name;
    }

protected:
    FallBackNameGen() : counter{0} {}

    size_t counter;
};

template<typename T>
FallBackNameGen<T> FallBackNameGen<T>::GEN{};


template<typename T>
class Scope {
public:
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairT;

    Scope() : lambdas{}, buffer{} {}

    void add_parameters(Lexeme<T> lexeme) {
        lambdas.push_back(make_pair(FallBackNameGen<T>::GEN.next(), make_shared<Lambda<T>>(lexeme.toString())));
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

    pairT clean_up() {
        if (buffer.size() != 1) {
            throw CompilerError("missing right brackets");
        }
        lambdas.back().second->set_root(move(buffer.back().second));
        for (auto i = lambdas.size(); i > 1; i--) {
            auto back = move(lambdas.back());
            lambdas.pop_back();
            lambdas.back().second->set_root(unique_ptr<Node<T>>{new Symbol<T>{back.first}});
            lambdas.back().second->add_lambda(move(back));
        }
        return move(lambdas.back());
    }

    void lambda_add(Lexeme<T> lexeme) {
        buffer.push_back(make_pair(lexeme, Root<T>{}));
    }

    vector<pairT> lambdas;
    vector<pair<Lexeme<T>, Root<T>>> buffer;
};


template<typename T>
class Parser {
public:
    explicit Parser(BasicFile<T> &file) :
            file{file}, stack{} {
        stack.push_back(Scope<T>{});
        stack.back().buffer.push_back(make_pair(Lexeme<T>{}, Root<T>{}));
    }

    Lambda<T> parse() {
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
                            stack.back().push(unique_ptr<Node<T>>{new Symbol<T>{lexeme.toString()}});
                        }
                        break;
                }
            }
        }
        return Lambda<T>{
                basic_string<T>{"args"},
                shared_ptr<Root<T>>{new Root{move(stack.back().buffer.back().second)}},
                stack.back().lambdas
        };
    }

    void right_brackets_resolve(Lexeme<T> lexeme) {
        auto lambda = stack.back().clean_up();
        stack.pop_back();
        stack.back().buffer.pop_back();       // pop lexeme lambda
        stack.back().buffer.back().second.push(unique_ptr<Node<T>>{new Symbol<T>{lambda.first}});
        stack.back().lambdas.push_back(move(lambda));
    }

protected:
    BasicFile<T> &file;
    vector<Scope<T>> stack;

};


#endif //CODEDOLLAR_PARSER_H
