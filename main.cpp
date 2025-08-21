#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    b.printBoard();
    MoveGen mg;
    mg.initAttackTables();
    std::vector<Move> moves = mg.generateAllMoves(b, WHITE);
    for (auto it : moves)
    {
        std::cout << it.from << ", " << it.to << "\n";
    }
    std::cout << moves.size();
}