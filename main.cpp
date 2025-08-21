#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    b.printBoard();
    MoveGen mg;
    std::vector<Move> bishopMoves = mg.generateBishopMoves(b.getBishops(WHITE), b.getOccupancy(WHITE), b.getOccupancy(BLACK));
    for (auto it : bishopMoves) {
        std::cout << it.from << ", " << it.to << "\n";
    }
    std::cout << bishopMoves.size();
}