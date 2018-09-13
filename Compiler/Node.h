//
// Created by cytosine on 18-9-9.
//

#ifndef CODEDOLLAR_NODE_H
#define CODEDOLLAR_NODE_H


#include <vector>
#include <memory>

using namespace std;

#include "Compiler.h"


template<typename T>
class Lambda;

template<typename T>
class Closure;


template<typename T>
class Context {
public:
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairT;

    Context(vector<vector<T>> &&link) : access_links{link} {}

    Context(basic_string<T> name, shared_ptr<Lambda<T>> value, vector<pairT> lambdas, Context<T> upper_layer) :
            access_links{lambdas.push_back(make_pair(name, value))} {}

    Context<T> closure(basic_string<T> name, unique_ptr<Lambda<T>> value, vector<pairT> lambdas) {
        return Context{name, value, lambdas, this};
    }

    Closure<T> find(basic_string<T> target) {
        auto e = access_links.rend();
        auto i = access_links.rbegin();
        shared_ptr<Lambda<T>> lambda;
        for (; i != e; --i) {
            for (auto each: *i) {
                if (each.first == target) {
                    lambda = each.second;
                    goto outer_loop;
                }
            }
        }
        outer_loop:

        return lambda->toClosure(vector<vector<T>>{++e, ++i});
    }

    Context<T> push(vector<pairT> link) {
        auto tmp = access_links;
        return tmp.push_back(link);
    }

    vector<vector<pairT>> access_links;
};


template<typename T>
class Node {
public:
    virtual Closure<T> calculate(Context<T> context) = 0;

protected:

};


template<typename T>
class CombineNode : public Node<T> {
public:
    CombineNode() : left{}, right{} {}

    CombineNode(unique_ptr<Node<T>> &&left, unique_ptr<Node<T>> &&right) :
            left{move(left)}, right{move(right)} {}

    virtual Closure<T> calculate(Context<T> context) final {
        return left->calculate(context)->call(right->calculate(context));
    }


protected:
    unique_ptr<Node<T>> left, right;
};


template<typename T>
class Symbol : public Node<T> {
public:
    Symbol(basic_string<T> target) : target{target} {}

    virtual Closure<T> calculate(Context<T> context) final {
        return context.find(target);
    }


protected:
    basic_string<T> target;
};


template<typename T>
class Root {
public:
    Root() : root{} {}

    explicit Root(unique_ptr<Node<T>> &&root) : root{move(root)} {}

    void push(unique_ptr<Node<T>> &&node) {
        if (root == nullptr) {
            root = move(node);
        } else {
            root = unique_ptr<Node<T>>{new CombineNode<T>{move(root), move(node)}};
        }
    }

    void push(Root<T> &&root) {
        push(move(root.root));
    }

    Closure<T> calculate(Context<T> context) {
        return root->calculate(context);
    }

protected:
    unique_ptr<Node<T>> root;

};


template<typename T>
class Lambda {
public:
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairT;

    Lambda(basic_string<T> argument) : argument{argument}, lambdas{}, root{} {}

    void set_root(Root<T> &&root) {
        this->root = move(root);
    }

    void add_lambda(unique_ptr<Lambda<T>> &&lambda) {
        lambdas.push_back(move(lambda));
    }

    Closure<T> toClosure(vector<vector<pairT>> access_link) {
        return Closure<T>{argument, root, access_link.push(lambdas)};
    }

protected:
    basic_string<T> argument;
    vector<pairT> lambdas;
    Root<T> root;
};


template<typename T>
class Closure {
public:
    Closure(basic_string<T> argument, Root<T> root, Context<T> access_link) :
            argument{argument}, root{root}, access_link{access_link} {}

    Closure call(Closure value) {
        return root.calculate(access_link.closure(argument, value));
    }

protected:
    basic_string<T> argument;
    Root<T> root;
    Context<T> access_link;
};


#endif //CODEDOLLAR_NODE_H
