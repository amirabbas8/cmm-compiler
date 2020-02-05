#ifndef MIPSER_PRIMITIVES
#define MIPSER_PRIMITIVES

#include "mipser.hpp"

#include <string>

class Variable : public Base {
private:
    std::string id;
    const Base *assign;
public:
    Variable(const std::string &_id) : id(_id) {}

    Variable(const std::string &_id, const Base *_assign) : id(_id), assign(_assign) {}

    virtual std::string analyze(Scope &scope) const {
        scope.pushSymbol(id, "id");
        std::cout << "//    Variable id = " << id << std::endl;
        if (assign != nullptr) {


            std::string res = assign->analyze(scope);
            if (res == "noop") {
                std::cerr << "Semantic error : i got confused. a function maybe is void but is used in a operation"
                          << std::endl;
                exit(1);
            }
            if (assign->type == "const") {
                std::cout << "addi $s1, $zero, " << res << std::endl;
                scope.setValue("$s1", "mips", std::stoi(res));
                res = "$s1";
            } else if (assign->type == "id") {
                const Symbol *symbolLeft = scope.getSymbol(res, assign->type);
                scope.setValue("$s1", "mips", symbolLeft->value);
                if (symbolLeft->type == "register") {
                    std::cout << "move $s1" << ", " << symbolLeft->address << std::endl;
                    res = "$s1";
                } else if (symbolLeft->type == "id") {
                    std::cout << "lw $s1" << ", " << symbolLeft->address << "($gp)" << std::endl;
                    res = "$s1";
                } else {
                    std::cerr << "Semantic error : " << assign->type << " " << res << " type is not correct!"
                              << std::endl;
                    exit(1);
                }

            }

            const Symbol *symbol = scope.getSymbol(id, "id");
            if (symbol->type == "register") {
                std::cout << "move " << symbol->address << ", " << res << std::endl;
                scope.setValue(symbol->name, "mips", scope.getValue("$s1", "mips"));
            } else if (symbol->type == "id") {
                std::cout << "sw " << res << ", " << symbol->address << "($gp)" << std::endl;
                scope.setValue(symbol->name, "id", scope.getValue("$s1", "mips"));
            } else {
                std::cerr << "Semantic error : " << "id" << " " << id << " type is not correct!" << std::endl;
                exit(1);
            }
            if (scope.isShow) {
                std::cout << "//show " << id << " = " << scope.getValue("$s1", "mips") << std::endl;
            }

        }
        return "noop";
    }

    virtual void push(const Base *var) const {
        std::cerr << "Error: Can't call this function on this class" << std::endl;
        (void) var;
    }
};

class Parameter : public Base {
public:
    Parameter(const std::string &_key, const std::string &_type) {
        type = _type;
        key = _key;
    }

    virtual std::string analyze(Scope &scope) const {
        (void) scope;
        return "register";
    }

    virtual void push(const Base *var) const {
        std::cerr << "Error: Can't call this function on this class" << std::endl;
        (void) var;
    }
};

#endif
