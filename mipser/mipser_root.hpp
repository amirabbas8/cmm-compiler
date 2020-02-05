#ifndef TOP_MIPSER
#define TOP_MIPSER

#include "mipser.hpp"

#include <vector>


class MipserRoot {
private:
    std::vector<const Base *> vec;
public:
    int lastAddr = 0;
    bool isShow = false;
    Scope *scope = new Scope(&lastAddr, isShow);

    MipserRoot(bool _isShow= false) : isShow(_isShow) {}

    void analyze() {
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i]->analyze(*scope);
        }
    }

    void push(const Base *stmnt) {
        vec.push_back(stmnt);
    }
};

#endif
