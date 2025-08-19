#include "Board.h"

#include <iostream>

int main() {
    Board b;
    b.loadStartPosition();
    b.printBoard();
    U64 a = b.wSinglePush();
    std::cout << "\n";
    b.printBoard();
    std::cout << "\n";
    std::cout << a;
    return 0;
}