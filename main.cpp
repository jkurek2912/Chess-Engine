#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    MoveGen mg;
    mg.initAttackTables();
    std::cout << mg.kingInCheck(b, WHITE) << "\n";
}