#ifndef MIPSER_DECLARATION
#define MIPSER_DECLARATION

#include "mipser.hpp"

#include <vector>


class DeclarationList : public Base {
private:
    mutable std::vector<const Base*> dec_list;

public:
    DeclarationList(const Base* _dec) {
	dec_list.push_back(_dec);
    }

    virtual std::string analyze(Scope & scope) const {
	for(size_t i = 0; i < dec_list.size(); ++i) {
	    dec_list[i]->analyze(scope);
	}
    return "noop";}

    virtual void push(const Base* _dec) const {
	dec_list.push_back(_dec);
    }
};

class VariableDeclaration : public Base {
private:
    mutable std::vector<const Base*> var_list;

public:
    VariableDeclaration(const Base* _var) {
	var_list.push_back(_var);
    }

    virtual std::string analyze(Scope & scope) const {
	for(size_t i = 0; i < var_list.size(); ++i) {
	    var_list[i]->analyze(scope);
	}
    return "noop";}

    virtual void push(const Base* _var) const {
	var_list.push_back(_var);
    }
};

#endif
