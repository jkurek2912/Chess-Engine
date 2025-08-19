#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main() {
    Board b;
    b.customSetBoard();
    MoveGen mg;
    std::vector<Move> moves = mg.generateWhitePawnPushes(b.getPawns(WHITE), b.getOccupancy(BOTH));    

}