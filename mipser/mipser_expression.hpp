#ifndef MIPSER_EXPRESSION
#define MIPSER_EXPRESSION

#include "mipser.hpp"

#include <string>
#include <iostream>

class Expression : public Base {
private:
public:
    Expression(const Base *_prevExpression,
               const Base *_nextExpression, const std::string &_type) {
        this->prevExpression = _prevExpression;
        this->nextExpression = _nextExpression;
        this->type = _type;
        this->prevExpression = _prevExpression;
    }

    Expression(const int _value) {
        this->value = _value;
        this->type = "const";
    }

    Expression() {}

    Expression(const std::string &_key) {
        this->key = _key;
        this->type = "id";
    }

    virtual std::string analyze(Scope &scope) const {
        if (type == "noop") {
            return "noop";
        } else if (type != "const" && type != "id") {
            std::string res1 = "", res2 = "", res1t = "", res2t = "";
            if (prevExpression != nullptr) {
                res1 = prevExpression->analyze(scope);
                if (res1 == "$s0") {
                    std::cout << "move $s1, " << res1 << std::endl;
                    scope.setValue("$s1", "mips", scope.getValue(res1));
                    res1 = "$s1";
                }
                res1t = prevExpression->type;
            }
            if (nextExpression != nullptr) {
                if (nextExpression->type != "id" && nextExpression->type != "const") {
                    std::cout << "\naddi $sp, $sp, -4" << std::endl;
                    std::cout << "sw  $s1, 0($sp)\n" << std::endl;
                }
                res2 = nextExpression->analyze(scope);
                if (nextExpression->type != "id" && nextExpression->type != "const") {
                    std::cout << "\nlw  $s1, 0($sp)" << std::endl;
                    std::cout << "addi $sp, $sp, 4\n" << std::endl;
                }
                if (res2 == "$s0") {
                    std::cout << "move $s2, " << res2 << std::endl;
                    scope.setValue("$s2", "mips", scope.getValue(res2));
                    res2 = "$s2";
                }
                res2t = nextExpression->type;
            }
            return format(scope, type, res1, res1t, res2, res2t);
        } else {
            return type == "id" ? key : std::to_string(value);
        }

    }

    virtual void push(const Base *_base) const {
        std::cerr << "Can't call this function for this type" << std::endl;
        (void) _base;
    }

    std::string
    format(Scope &scope, std::string t, std::string res1, std::string res1t, std::string res2,
           std::string res2t) const {
        if (res2 == "0" && t == "/") {
            std::cerr << "Semantic error :Oh my Gooooooooooooood! Division by zero !" << std::endl;
            exit(1);
        }
        if (res1 == "noop" || (res2 == "noop" && t != "func")) {
            std::cerr << "Semantic error : i got confused. a function maybe is void but is used in a operation"
                      << std::endl;
            exit(1);
        }
        if (t != "func" && t != "=" && res1t == "id") {
            const Symbol *symbol = scope.getSymbol(res1, res1t);
            scope.setValue("$s1", "mips", symbol->value);
            if (symbol->type == "register") {
                std::cout << "move $s1, " << symbol->address << std::endl;
            } else if (symbol->type == "id") {
                std::cout << "lw $s1, " << symbol->address << "($gp)" << std::endl;
            } else {
                std::cerr << "Semantic error : " << res1t << " " << res1 << " type is not correct!" << std::endl;
                exit(1);
            }
            res1 = "$s1";
        } else if (res1t == "const") {
            std::cout << "addi $s1, $zero, " << res1 << std::endl;
            scope.setValue("$s1", "mips", std::stoi(res1));
            res1 = "$s1";
        }
        if (t != "func" && res2t == "id") {
            const Symbol *symbol = scope.getSymbol(res2, res2t);
            scope.setValue("$s2", "mips", symbol->value);
            if (symbol->type == "register") {
                std::cout << "move $s2, " << symbol->address << std::endl;
            } else if (symbol->type == "id") {
                std::cout << "lw $s2, " << symbol->address << "($gp)" << std::endl;
            } else {
                std::cerr << "Semantic error : " << res2t << " " << res2 << " type is not correct!" << std::endl;
                exit(1);
            }
            res2 = "$s2";
        } else if (res2t == "const") {
            std::cout << "addi $s2, $zero, " << res2 << std::endl;
            scope.setValue("$s2", "mips", std::stoi(res2));
            res2 = "$s2";
        }

        if (t == "+") {
            std::cout << "add $s0, " << res1 << ", " << res2 << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) + scope.getValue(res2));
            return "$s0";
        }
        if (t == "-") {
            std::cout << "sub $s0, " << res1 << ", " << res2 << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) - scope.getValue(res2));
            return "$s0";
        }
        if (t == "&") {
            std::cout << "and $s0, " << res1 << ", " << res2 << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) & scope.getValue(res2));
            return "$s0";
        }
        if (t == "|") {
            std::cout << "or $s0, " << res1 << ", " << res2 << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) | scope.getValue(res2));
            return "$s0";
        }
        if (t == "~") {
            std::cout << "neg $s0, " << res2 << std::endl;
            scope.setValue("$s0", "mips", ~scope.getValue(res2));
            return "$s0";
        }
        if (t == "!") {
            std::string not_label_zero = get_temp_label();
            std::string not_label_end = get_temp_label();

            std::cout << "beq " << res2 << ", $zero, " << not_label_zero << std::endl;

            std::cout << "move $s0, $zero" << std::endl;
            std::cout << "j " << not_label_end << std::endl;

            std::cout << not_label_zero << ":" << std::endl;
            std::cout << "neg $s0, $zero" << std::endl;

            std::cout << not_label_end << ":" << std::endl;
            scope.setValue("$s0", "mips", !scope.getValue(res2));
            return "$s0";
        }
        if (t == "&&") {
            std::string log_and_label_false = get_temp_label();
            std::string log_and_label_end = get_temp_label();

            std::cout << "beq " << res1 << ", $zero, " << log_and_label_false << std::endl;
            std::cout << "beq " << res2 << ", $zero, " << log_and_label_false << std::endl;

            std::cout << "neg $s0, $zero" << std::endl;
            std::cout << "j " << log_and_label_end << std::endl;

            std::cout << log_and_label_false << ":" << std::endl;
            std::cout << "move $s0, $zero" << std::endl;

            std::cout << log_and_label_end << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) && scope.getValue(res2));
            return "$s0";
        }
        if (t == "||") {
            std::string log_or_label_true = get_temp_label();
            std::string log_or_label_end = get_temp_label();

            std::cout << "bne " << res1 << ", $zero, " << log_or_label_true << std::endl;
            std::cout << "bne " << res2 << ", $zero, " << log_or_label_true << std::endl;

            std::cout << "move $s0, $zero" << std::endl;
            std::cout << "j " << log_or_label_end << std::endl;

            std::cout << log_or_label_true << ":" << std::endl;
            std::cout << "neg $s0, $zero" << std::endl;

            std::cout << log_or_label_end << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) || scope.getValue(res2));
            return "$s0";
        }
        if (t == "*") {
            std::cout << "mult " << res1 << ", " << res2 << std::endl;
            std::cout << "mflo $s0" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) * scope.getValue(res2));
            return "$s0";
        }
        if (t == "/") {
            std::cout << "div $s0, " << res1 << ", " << res2 << std::endl;
            std::cout << "mflo $s0" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) / scope.getValue(res2));
            return "$s0";
        }
        if (t == "=") {
            const Symbol *symbol = scope.getSymbol(res1, res1t);
            if (symbol->type == "register") {
                std::cout << "move " << symbol->address << ", " << res2 << std::endl;
            } else if (symbol->type == "id") {
                std::cout << "sw " << res2 << ", " << symbol->address << "($gp)" << std::endl;
            } else {
                std::cerr << "Semantic error : " << res1t << " " << res1 << " type is not correct!" << std::endl;
                exit(1);
            }
            std::cout << "neg $s0, $zero" << std::endl;

            scope.setValue(symbol->name, symbol->type, scope.getValue(res2));
            if (scope.isShow) {
                std::cout << "//show " << res1 << "=" << scope.getValue(res2) << std::endl;
            }
            return "$s0";
        }
        if (t == "==") {
            std::string true_label = get_temp_label();
            std::string end_label = get_temp_label();

            std::cout << "beq " << res1 << ", " << res2 << ", " << true_label << std::endl;
            std::cout << "move " << "$s0" << ", $zero" << std::endl;
            std::cout << "j " << end_label << std::endl;
            std::cout << true_label << ":" << std::endl;
            std::cout << "neg " << "$s0" << ", $zero" << std::endl;
            std::cout << end_label << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) == scope.getValue(res2));
            return "$s0";
        }
        if (t == "!=") {
            std::string true_label = get_temp_label();
            std::string end_label = get_temp_label();

            std::cout << "bne " << res1 << ", " << res2 << ", " << true_label << std::endl;
            std::cout << "move " << "$s0" << ", $zero" << std::endl;
            std::cout << "j " << end_label << std::endl;
            std::cout << true_label << ":" << std::endl;
            std::cout << "neg " << "$s0" << ", $zero" << std::endl;
            std::cout << end_label << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) != scope.getValue(res2));
            return "$s0";
        }
        if (t == ">") {
            std::cout << "slt " << "$s0" << ", " << res2 << ", " << res1 << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) > scope.getValue(res2));
            return "$s0";
        }
        if (t == ">=") {
            std::string true_label = get_temp_label();
            std::string end_label = get_temp_label();

            std::cout << "beq " << res1 << ", " << res2 << ", " << true_label << std::endl;
            std::cout << "slt " << "$s0" << ", " << res2 << ", " << res1 << std::endl;
            std::cout << "bne " << "$s0" << ", $zero, " << true_label << std::endl;
            std::cout << "move " << res1 << ", $zero" << std::endl;
            std::cout << "j " << end_label << std::endl;
            std::cout << true_label << ":" << std::endl;
            std::cout << "neg " << res1 << ", $zero" << std::endl;
            std::cout << end_label << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) >= scope.getValue(res2));
            return "$s0";
        }
        if (t == "<") {
            std::cout << "slt " << "$s0" << ", " << res1 << ", " << res2 << std::endl;
            return "$s0";
        }
        if (t == "<=") {
            std::string true_label = get_temp_label();
            std::string end_label = get_temp_label();

            std::cout << "beq " << res1 << ", " << res2 << ", " << true_label << std::endl;
            std::cout << "slt " << "$s0" << ", " << res1 << ", " << res2 << std::endl;
            std::cout << "bne " << "$s0" << ", $zero, " << true_label << std::endl;
            std::cout << "move " << "$s0" << ", $zero" << std::endl;
            std::cout << "j " << end_label << std::endl;
            std::cout << true_label << ":" << std::endl;
            std::cout << "neg " << "$s0" << ", $zero" << std::endl;
            std::cout << end_label << ":" << std::endl;
            scope.setValue("$s0", "mips", scope.getValue(res1) <= scope.getValue(res2));
            return "$s0";
        }
        if (t == "func") {
            const Symbol *func = scope.getSymbol(res1, "func");
            std::cout << "jal " << res1 << std::endl;
            std::cout << "lw  s2, 36($sp)" << std::endl;
            std::cout << "lw  s1, 32($sp)" << std::endl;
            std::cout << "lw  s0, 28($sp)" << std::endl;
            std::cout << "lw  a3, 24($sp)" << std::endl;
            std::cout << "lw  a2, 20($sp)" << std::endl;
            std::cout << "lw  a1, 16($sp)" << std::endl;
            std::cout << "lw  a0, 12($sp)" << std::endl;
            std::cout << "lw  $ra, 8($sp)" << std::endl;
            std::cout << "lw  $v1, 4($sp)" << std::endl;
            std::cout << "lw  $v0, 0($sp)" << std::endl;
            std::cout << "addi $sp, $sp, 40" << std::endl;
            std::cout << "//end of function call" << std::endl;
            if (func->return_type == "int") {
                return "$v0";
            } else {
                return "noop";
            }
        }


        return "noop";
    }

};


#endif
