//
// Created by cytosine on 18-9-6.
//

#ifndef CODEDOLLAR_TREE_H
#define CODEDOLLAR_TREE_H


#include <vector>
#include <iterator>
#include <memory>

using namespace std;


template<typename T>
class TreeIter : iterator<forward_iterator_tag, vector<unique_ptr<T>>> {
public:
    typedef typename vector<unique_ptr<T>>::const_iterator iterT;

    explicit TreeIter(iterT ptr) : ptr{ptr} {}

    TreeIter(const TreeIter &other) = default;

    TreeIter &operator=(const TreeIter &other) = default;

    TreeIter &operator++() {
        ptr++;
        return *this;
    }

    const TreeIter operator++(int) {
        const TreeIter tmp{*this};
        ptr++;
        return tmp;
    }

    const T &operator*() {
        return **ptr;
    }

    const T &operator->() {
        return **ptr;
    }

    bool operator==(const TreeIter &other) const {
        return ptr == other.ptr;
    }

    bool operator!=(const TreeIter &other) const {
        return ptr != other.ptr;
    }

protected:
    iterT ptr;

};


template<typename T>
class TreeIterable {
public:
    typedef vector<unique_ptr<T>> vectorT;

    explicit TreeIterable(const vectorT &vec) : vec{vec} {}

    TreeIter<T> begin() {
        return TreeIter<T>{vec.begin()};
    }

    TreeIter<T> end() {
        return TreeIter<T>{vec.end()};
    }

protected:
    const vectorT &vec;

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

    TreeIterable<T> iter() const {
        return TreeIterable<T>{subNodes};
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

    const T &operator->() {
        return **ptr;
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

protected:
    const vectorT &vec;

};


#endif //CODEDOLLAR_TREE_H
