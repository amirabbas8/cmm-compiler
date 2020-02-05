#ifndef MIPSER_BASE
#define MIPSER_BASE

#include <string>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <iomanip>

class Symbol {
public:
    std::string name;
    std::string scope;
    std::string type;
    std::string address;
    std::string return_type = "null";
    std::vector<const std::string *> param;
    mutable int value = 0;

    Symbol(std::string _name, std::string _scope, std::string _type,
           std::string _address, std::string _return_type,
           std::vector<const std::string *> _param) : name(_name), scope(_scope),
                                                      type(_type),
                                                      address(_address),
                                                      return_type(_return_type),
                                                      param(_param) {}
};

class Scope {
private:
    int *lastAddr;
    const Scope *parent = nullptr;
public:
    bool isShow = false;
    std::string scope_name = "global";
    std::vector<const Scope *> children;
    std::vector<const Symbol *> table;
    std::string return_type = "noop";


    Scope(int *_lastAddr, bool _isShow = false) {
        lastAddr = _lastAddr;
        isShow = _isShow;
    }

    Scope(std::string _scope_name, int *_lastAddr, std::string _return_type, bool _isShow = false) : scope_name(
            _scope_name) {
        lastAddr = _lastAddr;
        return_type = _return_type;
        isShow = _isShow;
    }

    virtual void pushSymbol(std::string name, std::string type = "id", std::string address = "default",
                            std::string return_type = "null",
                            std::vector<const std::string *> param = *(new std::vector<const std::string *>())) {
        if (address == "default") {
            address = std::to_string(*lastAddr);
            *lastAddr = (*lastAddr) + 4;
        }
        bool isID = (type == "id");
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i]->name == name &&
                (table[i]->type == type || (isID && table[i]->type == "register") || table[i]->type == "keyword")) {
                std::cerr << "Semantic error : " << type << " " << name << " already exists in " << scope_name
                          << " scope!" << std::endl;
                exit(1);
            }
        }
        table.push_back(new Symbol(name, scope_name, type, address, return_type, param));
    }

    virtual const Symbol *getSymbol(std::string name, std::string type) const {
//        std::cout << name << type << scope_name;
        bool isID = (type == "id");
        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i]->name == name && (table[i]->type == type || (isID && table[i]->type == "register"))) {
                return table[i];
            }
        }
        if (parent) {
            return parent->getSymbol(name, type);
        }
        std::cerr << "Semantic error : " << type << " " << name << " not found!" << std::endl;
        exit(1);
    }

    virtual void setValue(std::string name, std::string type, int value) const {
        if (isShow) {
//            std::cerr << name << type << std::to_string(value) << "set" << std::endl;
            getSymbol(name, type)->value = value;
        }
    }

    virtual int getValue(std::string name, std::string type = "mips") const {

//        std::cerr << name << type << std::to_string(getSymbol(name, type)->value) << "get" << std::endl;

        return getSymbol(name, type)->value;
    }

    virtual Scope *push_scope(std::string name, std::string _return_type) {
        Scope *child = new Scope(name, lastAddr, _return_type, isShow);
        child->parent = this;
        children.push_back(child);
        return child;
    }

    virtual void printTable() const {
        for (size_t i = 0; i < table.size(); ++i) {
            std::cout << "//"
                      << std::setw(15) << table[i]->name << " "
                      << std::setw(20) << table[i]->scope << " "
                      << std::setw(10) << table[i]->type << " "
                      << std::setw(20) << table[i]->address << " "
                      << std::setw(10) << std::to_string(table[i]->value) << " "
                      << std::setw(10) << table[i]->return_type;
            for (size_t j = 0; j < table[i]->param.size(); ++j) {
                std::cout << " " << std::setw(10) << *(table[i]->param[j]) << " ";
            }
            std::cout << std::endl;
        }
        for (size_t i = 0; i < children.size(); ++i) {
            children[i]->printTable();
        }
    }
};

class Base {
public:
    std::string type = "noop";
    int value;
    std::string key;
    const Base *prevExpression;
    const Base *nextExpression;

    virtual ~Base() {}

    virtual std::string analyze(Scope &scope) const = 0;

    virtual void push(const Base *_var) const = 0;

    std::string get_temp_label() const {
        int r = rand() % 100;
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int ms = tp.tv_sec + tp.tv_usec / 1000;
        return "l" + std::to_string(ms + r);
    }
};

#endif
