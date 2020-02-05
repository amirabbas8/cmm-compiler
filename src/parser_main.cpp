#include "mipser.hpp"

#include <iostream>
#include <cstring>


int main(int argc, char *argv[]) {
    MipserRoot *mipser;
    bool isShow = false;
    if (argc >= 2 && !strcmp(argv[1], "--show")) {
        isShow = true;
        std::cout << "// in show mode" << std::endl;
        mipser = parseMIPSER(isShow);
    } else {
        mipser = parseMIPSER();
    }
    std::cout << "jal main" << std::endl;
    std::cout << "exit:" << std::endl;
    std::cout << "j exit" << std::endl;

    mipser->scope->pushSymbol("$v0", "mips", "", "");
    mipser->scope->pushSymbol("$v1", "mips", "", "");
    mipser->scope->pushSymbol("$a0", "mips", "", "");
    mipser->scope->pushSymbol("$a1", "mips", "", "");
    mipser->scope->pushSymbol("$a2", "mips", "", "");
    mipser->scope->pushSymbol("$a3", "mips", "", "");
    mipser->scope->pushSymbol("$s0", "mips", "", "");
    mipser->scope->pushSymbol("$s1", "mips", "", "");
    mipser->scope->pushSymbol("$s2", "mips", "", "");
    mipser->scope->pushSymbol("if", "keyword", "keyword");
    mipser->scope->pushSymbol("else", "keyword", "keyword");
    mipser->scope->pushSymbol("int", "keyword", "keyword");
    mipser->scope->pushSymbol("while", "keyword", "keyword");
    mipser->scope->pushSymbol("for", "keyword", "keyword");
    mipser->scope->pushSymbol("return", "keyword", "keyword");
    mipser->scope->pushSymbol("exit", "func", "exit");
    mipser->analyze();
    mipser->scope->getSymbol("main", "func");

    if (isShow) {
        std::cout << "// symbol table" << std::endl;
        std::cout << "//"
                  << std::setw(15) << "name"
                  << std::setw(20) << "scope"
                  << std::setw(10) << "type"
                  << std::setw(25) << "address"
                  << std::setw(10) << "value"
                  << std::setw(15) << "return_type"
                  << std::setw(15) << "params..." << std::endl;
        mipser->scope->printTable();
        std::cout << "//count of variables: " << std::to_string(mipser->lastAddr / 4) <<
                  ", number of used stack rows: " << std::to_string(mipser->lastAddr) << std::endl;
    }

    return 0;
}
