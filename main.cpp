#include "Board.h"
#include <iostream>
int main()
{
    Board b;
    b.setBoard();
    b.printBoard();
    std::cout << "\n";
    b.clearBoard();
    b.printBoard();
}