#ifndef MIPSER_FUNCTION
#define MIPSER_FUNCTION

#include "mipser.hpp"

#include <string>
#include <iostream>

class ParamList : public Base {
private:
    bool isInCall = false;

public:
    mutable std::string funcName = "mai2n";
    mutable std::vector<const Base *> param_list;


    ParamList(bool _isInCall = false) {
        isInCall = _isInCall;
    }

    ParamList(const Base *param, bool _isInCall = false) {
        param_list.push_back(param);
        isInCall = _isInCall;
    }

    virtual std::string analyze(Scope &scope) const {
        if (isInCall) {
            return printInCall(scope);
        }
        for (size_t i = 0; i < param_list.size(); ++i) {
            std::cout << "// Parameter = " << param_list[i]->key << std::endl;
            scope.pushSymbol(param_list[i]->key, "register", "$a" + std::to_string(i));
        }
        return "noop";
    }

    std::string printInCall(Scope &scope) const {
        std::cout << "//start of function call" << std::endl;
        std::cout << "addi $sp, $sp, -40" << std::endl;
        std::cout << "sw  $v0, 0($sp)" << std::endl;
        std::cout << "sw  $v1, 4($sp)" << std::endl;
        std::cout << "sw  $ra, 8($sp)" << std::endl;
        std::cout << "sw  a0, 12($sp)" << std::endl;
        std::cout << "sw  a1, 16($sp)" << std::endl;
        std::cout << "sw  a2, 20($sp)" << std::endl;
        std::cout << "sw  a3, 24($sp)" << std::endl;
        std::cout << "sw  s0, 28($sp)" << std::endl;
        std::cout << "sw  s1, 32($sp)" << std::endl;
        std::cout << "sw  s2, 36($sp)" << std::endl;

        const Symbol *func = scope.getSymbol(funcName, "func");
        if (func->param.size() != param_list.size()) {
            std::cerr << "Semantic error : " << funcName << " must have " << func->param.size() << " variables!"
                      << std::endl;
            exit(1);
        }
        for (size_t i = 0; i < param_list.size(); ++i) {
            std::string res = param_list[i]->analyze(scope);
            std::cout << "add  $s" << std::to_string(i + 3) << ", $zero, " << res << std::endl;
        }
        for (size_t i = 0; i < param_list.size(); ++i) {
            std::cout << "add  $a" << std::to_string(i) << ", $zero, " << " $s" << std::to_string(i + 3) << std::endl;
        }
        return "noop";
    }

    virtual void push(const Base *_var) const {
        param_list.push_back(_var);
    }
};

class Function : public Base {
private:
    std::string id;
    const ParamList *param;
    const Base *comp_statement;
    std::string return_type;
public:
    Function(const std::string &_id, const Base *_param, const Base *_comp_statement, std::string &_return_type) :
            id(_id), param((ParamList *) _param), comp_statement(_comp_statement), return_type(_return_type) {}

    virtual std::string analyze(Scope &scope) const {
        std::vector<const std::string *> paramTypes;
        for (size_t i = 0; i < param->param_list.size(); ++i) {
            paramTypes.push_back(&(param->param_list[i]->type));
        }
        scope.pushSymbol(id, "func", id, return_type, paramTypes);
        std::cout << id << ": " << std::endl;
        Scope *child = scope.push_scope(id, return_type);
        param->analyze(*child);
        comp_statement->analyze(*child);
        std::cout << "jr $ra" << std::endl;
        return "noop";
    }

    virtual void push(const Base *var) const {
        std::cerr << "Error: Can't call this function on this class" << std::endl;
        (void) var;
    }
};


#endif
