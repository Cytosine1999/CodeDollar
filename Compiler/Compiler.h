//
// Created by Cytosine on 2018/9/4.
//

#ifndef CODEDOLLAR_COMPILER_H
#define CODEDOLLAR_COMPILER_H

#include <stdexcept>

using namespace std;

class CompilerException : public runtime_error {
public:
    explicit CompilerException(const char *msg) : runtime_error(msg) {}
};

namespace Compiler {

}


#endif //CODEDOLLAR_COMPILER_H
