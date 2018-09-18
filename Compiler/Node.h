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
class Context;


template<typename T>
class LocalData {
public:
    typedef pair<basic_string<T>, Closure<T>> pairArgs;
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairLambda;

    explicit LocalData(basic_string<T> argument, vector<pairLambda> access_link) :
            argument{make_pair(argument, Closure<T>{})}, access_link{access_link} {}

    void pass_argument(Closure<T> value) {
        argument.second = value;
    }


    pairArgs argument;
    vector<pairLambda> access_link;

};


template<typename T>
class Context {
public:
    typedef pair<basic_string<T>, shared_ptr<Closure<T>>> pairArgs;
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairLambda;

    Context() : access_links{} {}

    explicit Context(vector<LocalData<T>> &&link) : access_links{move(link)} {}

    Closure<T> find(basic_string<T> target) {
        auto i = access_links.end();
        auto e = access_links.begin();
        while (i-- != e) {
            if (i->argument.first == target) {
                return i->argument.second;
            }
            for (auto each: i->access_link) {
                if (each.first == target) {
                    return each.second->toClosure(Context{vector<LocalData<T>>{e, ++i}});
                }
            }
        }
        throw CompilerError("unbind symbol");
    }

    Context<T> push(basic_string<T> argument, vector<pairLambda> data) {
        auto tmp = access_links;
        tmp.push_back(LocalData<T>{argument, data});
        return Context<T>{move(tmp)};
    }

    Context<T> pass_argument(Closure<T> value) {
        auto tmp = access_links;
        tmp.back().pass_argument(value);
        return Context<T>{move(tmp)};
    }

    LocalData<T> back() {
        return access_links.back();
    }

    // for test only
    basic_string<T> get_args() {
        return access_links.back().argument.first;
    }

protected:
    vector<LocalData<T>> access_links;
};


template<typename T>
class Node {
public:
    virtual Closure<T> calculate(Context<T> context) = 0;

    virtual basic_string<T> toString(Context<T> context, bool flag) = 0;

protected:

};


template<typename T>
class CombineNode : public Node<T> {
public:
    CombineNode() : Node<T>{}, left{}, right{} {}

    CombineNode(unique_ptr<Node<T>> &&left, unique_ptr<Node<T>> &&right) :
            left{move(left)}, right{move(right)} {}

    virtual Closure<T> calculate(Context<T> context) final {
        return left->calculate(context).call(right->calculate(context));
    }

    virtual basic_string<T> toString(Context<T> context, bool flag) final {
        if (flag) {
            return"(" + left->toString(context, false) + " " + right->toString(context, true) + ")";
        } else {
            return left->toString(context, false) + " " + right->toString(context, true);
        }
    }

protected:
    unique_ptr<Node<T>> left, right;
};


template<typename T>
class Symbol : public Node<T> {
public:
    explicit Symbol(basic_string<T> target) : Node<T>{}, target{target} {}

    virtual Closure<T> calculate(Context<T> context) final {
        return context.find(target);
    }

    virtual basic_string<T> toString(Context<T> context, bool flag) final {
        auto tmp = context.find(target);
        if (tmp.if_init()) {
            return tmp.toString();
        } else {
            return target;
        }
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

    // for test only
    basic_string<T> toString(Context<T> context) {
        return root->toString(context, false);
    }

protected:
    unique_ptr<Node<T>> root;

};


template<typename T>
class Lambda {
public:
    typedef pair<basic_string<T>, shared_ptr<Lambda<T>>> pairT;

    explicit Lambda(basic_string<T> argument) : argument{argument}, root{}, lambdas{} {}

    Lambda(basic_string<T> argument, shared_ptr<Root<T>> root, vector<pairT> lambdas) :
            argument{argument}, root{root}, lambdas{lambdas} {}

    void set_root(Root<T> &&root) {
        this->root = make_shared<Root<T>>(move(root));
    }

    void set_root(unique_ptr<Node<T>> &&root) {
        this->root = make_shared<Root<T>>(move(root));
    }

    void set_lambdas(vector<pairT> lambdas) {
        this->lambdas = lambdas;
    }

    void add_lambda(pairT &&lambda) {
        lambdas.push_back(move(lambda));
    }

    Closure<T> toClosure(Context<T> access_link) {
        return Closure<T>{root, access_link.push(argument, lambdas)};
    }

    // this function is temporary and only for test
    Closure<T> calculate() {
        return toClosure(Context<T>{}).call(Closure<T>());
    }

protected:
    basic_string<T> argument;
    shared_ptr<Root<T>> root;
    vector<pairT> lambdas;

};


template<typename T>
class Closure {
public:
    Closure() : root{}, access_link{} {}

    Closure(shared_ptr<Root<T>> root, Context<T> access_link) :
            root{root}, access_link{access_link} {}

    Closure call(Closure value) {
        return root->calculate(access_link.pass_argument(value));
    }

    shared_ptr<Lambda<T>> toLambda() {
        return make_shared<Lambda<T>>(root, access_link.back());
    }

    // for test only
    basic_string<T> toString() {


        return "${" + access_link.get_args() + ": " + root->toString(access_link) + "}";
    }

    bool if_init() {
        return root != nullptr;
    }

protected:
    shared_ptr<Root<T>> root;
    Context<T> access_link;
};


#endif //CODEDOLLAR_NODE_H
