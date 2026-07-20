#include "bitboard.hpp"
#include "uci.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        init_attack_tables(argc > 0 ? argv[0] : "");
    } catch (const std::exception& error) {
        std::cout << "info string initialization failed: " << error.what() << std::endl;
        return 1;
    }
    return run_uci();
}
