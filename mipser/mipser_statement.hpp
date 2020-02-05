#ifndef MIPSER_STATEMENT
#define MIPSER_STATEMENT

#include "mipser.hpp"

class Statement : public Base {
protected:
    mutable std::vector<const Base *> list;

public:
    Statement() {}

    Statement(const Base *_el) {
        list.push_back(_el);
    }

    Statement(const Base *_dec, const Base *_statement) {
        list.push_back(_dec);
        list.push_back(_statement);
    }

    virtual std::string analyze(Scope &scope) const {
        for (size_t i = 0; i < list.size(); ++i) {
            list[i]->analyze(scope);
        }
        return "noop";
    }

    virtual void push(const Base *_var) const {
        list.push_back(_var);
    }
};


class ScopeStatement : public Statement {
public:
    ScopeStatement() : Statement() {}

    ScopeStatement(const Base *_el) : Statement(_el) {}

    ScopeStatement(const Base *_dec, const Base *_statement) :
            Statement(_dec, _statement) {}

    virtual std::string analyze(Scope &scope) const override {
        Scope *child = scope.push_scope("scope_" + get_temp_label(), scope.return_type);
        std::cout << "//" << child->scope_name << std::endl;
        for (size_t i = 0; i < list.size(); ++i) {
            list[i]->analyze(*child);
        }
        std::cout << "//" << child->scope_name << std::endl;
        return "noop";
    }
};

class IfStatement : public Statement {
public:

    IfStatement(const Base *_expr, const Base *_el) {
        list.push_back(_expr);
        list.push_back(_el);
    }

    IfStatement(const Base *_expr, const Base *_if, const Base *_else) {

        list.push_back(_expr);
        list.push_back(_if);
        list.push_back(_else);
    }


    virtual std::string analyze(Scope &scope) const {
        std::string condition = (list[0]->analyze(scope));
        std::string end_if = get_temp_label();
        std::cout << "beq " << condition << ", $zero, " << end_if << std::endl;
        list[1]->analyze(scope);
        if (list.size() > 2) {
            std::string end_else = get_temp_label();
            std::cout << "j " << end_else << std::endl;
            std::cout << end_if << ":" << std::endl;
            list[2]->analyze(scope);
            std::cout << end_else << ":" << std::endl;
        } else {
            std::cout << end_if << ":" << std::endl;
        }


        return "noop";
    }


};

class WhileStatement : public Statement {
public:
    WhileStatement(const Base *_expr, const Base *_stmt) {
        list.push_back(_expr);
        list.push_back(_stmt);
    }


    virtual std::string analyze(Scope &scope) const {
        std::string condition = get_temp_label();
        std::string end_while = get_temp_label();
        std::cout << condition << ":" << std::endl;
        std::string conditionRes = list[0]->analyze(scope);
        std::cout << "beq " << conditionRes << ", $zero, " << end_while << std::endl;
        list[1]->analyze(scope);
        std::cout << "j " << condition << std::endl;
        std::cout << end_while << ":" << std::endl;

        return "noop";
    }


};

class ForStatement : public Statement {
public:
    ForStatement(const Base *_first, const Base *_begin, const Base *_end, const Base *_stmt) {
        list.push_back(_first);
        list.push_back(_begin);
        list.push_back(_end);
        list.push_back(_stmt);
    }


    virtual std::string analyze(Scope &scope) const {
        list[0]->analyze(scope);
        std::string condition = get_temp_label();
        std::string end_for = get_temp_label();
        std::cout << condition << ":" << std::endl;
        std::string conditionRes = list[1]->analyze(scope);
        std::cout << "beq " << conditionRes << ", $zero, " << end_for << std::endl;
        list[3]->analyze(scope);
        list[2]->analyze(scope);
        std::cout << "j " << condition << std::endl;
        std::cout << end_for << ":" << std::endl;

        return "noop";
    }


};


class ReturnStatement : public Statement {
public:
    ReturnStatement() : Statement() {}

    ReturnStatement(const Base *_el) : Statement(_el) {}

    virtual std::string analyze(Scope &scope) const {
        std::string res = "noop";
        if (list.size() > 0) {
            res = list[0]->analyze(scope);
            if (list[0]->type == "const") {
                std::cout << "addi $s1, $zero, " << res << std::endl;
                scope.setValue("$s1", "mips", std::stoi(res));
                res = "$s1";
            } else if (list[0]->type == "id") {
                const Symbol *symbol = scope.getSymbol(res, list[0]->type);
                scope.setValue("$s1", "mips", symbol->value);
                if (symbol->type == "register") {
                    std::cout << "move $s1, " << symbol->address << std::endl;
                } else if (symbol->type == "id") {
                    std::cout << "lw $s1, " << symbol->address << "($gp)" << std::endl;
                } else {
                    std::cerr << "Semantic error : " << res << " " << list[0]->type << " type is not correct!"
                              << std::endl;
                    exit(1);
                }
                res = "$s1";
            }
        }
        if (res != "noop") {
            std::cout << "move $v0, " << res << std::endl;
            scope.setValue("$v0", "mips", scope.getValue("$s1", "mips"));
        }

        if ((scope.return_type == "int" && res == "noop") || (scope.return_type == "void" && res != "noop")) {
            std::cerr << "Semantic error : you can't return that!" << std::endl;
            exit(1);
        }
        std::cout << "jr $ra" << std::endl;
        return "noop";
    }
};

#endif
