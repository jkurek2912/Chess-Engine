#include "Board.h"
#include "MoveGen.h"

#include <iostream>

int main() {
    MoveGen mg;
    mg.initPawnAttacks();
    std::cout << mg.arrPawnAttacks[WHITE][36];
}