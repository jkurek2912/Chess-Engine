#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    MoveGen mg;
    mg.initKnightAttacks();
    std::vector<Move> moves = mg.generateWhiteKnightMoves(b.getKnights(WHITE), b.getOccupancy(WHITE));
    for (auto it : moves)
    {
        std::cout << it.from << ", " << it.to << "\n";
    }
}