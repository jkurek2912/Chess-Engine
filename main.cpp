#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main()
{
    Board b;
    b.customSetBoard();
    MoveGen mg;
    mg.initAttackTables();
    std::vector<Move> moves = mg.generateAllLegalMoves(b, WHITE);
    std::cout << b.getCastlingRights()[0] << "\n";
    for (auto move : moves) {
         std::cout << move.from << " " << move.to << " " << move.kingsideCastle << " " << move.queensideCastle << "\n";
    }
}