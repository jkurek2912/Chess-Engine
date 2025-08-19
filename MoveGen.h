#pragma once
#include "Board.h"
#include <vector>

class Move
{
public:
    int from;
    int to;

    Move(int f, int t) : from(f), to(t) {}
};

class MoveGen
{
public:
    static void initPawnAttacks();
    static U64 pawnAttacks(int color, int square);

    static std::vector<Move> generateAllMoves(const Board &board);

    static U64 whiteSinglePushTargets(U64 whitePawns, U64 occupancy);
    static U64 whiteDoublePushTargets(U64 whitePawns, U64 occupancy);
    static U64 blackSinglePushTargets(U64 blackPawns, U64 occupancy);
    static U64 blackDoublePushTargets(U64 blackPawns, U64 occupancy);

    static std::vector<Move> generateWhitePawnPushes(U64 whitePawns, U64 occupancy);
    static std::vector<Move> generateBlackPawnPushes(U64 blackPawns, U64 occupancy);

    static std::vector<Move> generateWhitePawnCaptures(U64 whitePawns, U64 blackOccupancy);
    static std::vector<Move> generateBlackPawnCaptures(U64 blackPawns, U64 whiteOccupancy);

    static std::vector<Move> generateWhiteKnightMoves(U64 whiteKnights, U64 whiteOccupancy, U64 blackOccupancy);
    static std::vector<Move> generateBlackKnightMoves(U64 blackKnights, U64 blackOccupancy, U64 whiteOccupancy);

private:
    static U64 arrPawnAttacks[2][64];
};