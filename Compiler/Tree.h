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
class Tree {
public:
    typedef T typeT;
    typedef unique_ptr<T> typeT_ptr;
    typedef typename vector<typeT_ptr>::const_iterator const_iterator;

    explicit Tree() : subNodes{} {}

    Tree(const Tree &other) = default;

    Tree &operator=(const Tree &other) = default;

    size_t size() const {
        return subNodes.size();
    }

    const_iterator begin() const {
        return subNodes.begin();
    }

    const_iterator end() const {
        return subNodes.end();
    }

    virtual ~Tree() = default;

protected:
    vector<typeT_ptr> subNodes;

    void push_back(typeT *element) {
        subNodes.push_back(typeT_ptr{element});
    }
};



#endif //CODEDOLLAR_TREE_H
