#include "Board.h"

#include <iostream>

int main() {
    Board b;
    b.printBoard();
    std::cout << "\n";
    b.customSetBoard();
    b.printBoard();
    std::cout << "\n";
    b.loadStartPosition();
    b.printBoard();
    return 0;
}