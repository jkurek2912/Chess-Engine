#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    MoveGen mg;
    mg.initPawnAttacks();
    std::vector<Move> moves = mg.generateWhitePawnAttacks(b.getPawns(WHITE), b.getOccupancy(BLACK));
    for (auto it : moves)
    {
        std::cout << it.from << ", " << it.to << "\n";
    }
}