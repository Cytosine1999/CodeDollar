//
// Created by cytosine on 18-9-6.
//

#ifndef CODEDOLLAR_TREE_H
#define CODEDOLLAR_TREE_H


#include "iostream"
#include <vector>
#include <iterator>
#include <memory>

using namespace std;


template<typename T>
class uniquePtrIter : iterator<forward_iterator_tag, vector<unique_ptr<T>>> {
public:
    typedef typename vector<unique_ptr<T>>::const_iterator iterT;

    explicit uniquePtrIter(iterT ptr) : ptr{ptr} {}

    uniquePtrIter(const uniquePtrIter &other) = default;

    uniquePtrIter &operator=(const uniquePtrIter &other) = default;

    uniquePtrIter &operator++() {
        ptr++;
        return *this;
    }

    const uniquePtrIter operator++(int) {
        const uniquePtrIter tmp{*this};
        ptr++;
        return tmp;
    }

    const T &operator*() {
        return **ptr;
    }

    const unique_ptr<T> &operator->() {
        return *ptr;
    }

    bool operator==(const uniquePtrIter &other) const {
        return ptr == other.ptr;
    }

    bool operator!=(const uniquePtrIter &other) const {
        return ptr != other.ptr;
    }

protected:
    iterT ptr;

};


template<typename valueT, typename ownerT>
class uniquePtrIterable {
public:
    typedef vector<unique_ptr<valueT>> vectorT;

    explicit uniquePtrIterable(const vectorT &vec, ownerT &owner) : vec{vec}, owner{owner} {}

    uniquePtrIter<valueT> begin() {
        return uniquePtrIter<valueT>{vec.begin()};
    }

    uniquePtrIter<valueT> end() {
        return uniquePtrIter<valueT>{vec.end()};
    }

    bool notEnd(uniquePtrIter<valueT> ptr) {
        return ptr != end() || owner.pushBack();
    }

protected:
    const vectorT &vec;
    ownerT &owner;

};


template<typename T>
class Tree {
public:
    typedef T typeT;
    typedef unique_ptr<T> typeT_ptr;

    explicit Tree() : subNodes{} {}

    Tree(const Tree &other) = default;

    Tree &operator=(const Tree &other) = default;

    size_t size() const {
        return subNodes.size();
    }

    uniquePtrIterable<T, Tree<T>> iter() {
        return uniquePtrIterable<T, Tree<T>>{subNodes, *this};
    }

    virtual bool load() = 0;

    T &back() {
        return *subNodes.back();
    }

    virtual ~Tree() = default;

protected:
    vector<typeT_ptr> subNodes;

    void push_back(typeT *element) {
        subNodes.push_back(typeT_ptr{element});
    }
};


template<typename T>
class RefVecIter : iterator<forward_iterator_tag, vector<const T *>> {
public:
    typedef typename vector<const T *>::const_iterator iterT;

    explicit RefVecIter(iterT ptr) : ptr{ptr} {}

    RefVecIter(const RefVecIter &other) = default;

    RefVecIter &operator=(const RefVecIter &other) = default;

    RefVecIter &operator++() {
        ptr++;
        return *this;
    }

    const RefVecIter operator++(int) {
        const RefVecIter tmp{*this};
        ptr++;
        return tmp;
    }

    const T &operator*() {
        return **ptr;
    }

    const T *operator->() {
        return *ptr;
    }

    bool operator==(const RefVecIter &other) const {
        return ptr == other.ptr;
    }

    bool operator!=(const RefVecIter &other) const {
        return ptr != other.ptr;
    }

protected:
    iterT ptr;

};


template<typename T>
class RefVecIterable {
public:
    typedef vector<const T *> vectorT;

    explicit RefVecIterable(const vectorT &vec) : vec{vec} {}

    RefVecIter<T> begin() {
        return RefVecIter<T>{vec.begin()};
    }

    RefVecIter<T> end() {
        return RefVecIter<T>{vec.end()};
    }

    bool notEnd(RefVecIter<T> ptr) {
        return ptr != end();
    }

protected:
    const vectorT &vec;

};


#endif //CODEDOLLAR_TREE_H
