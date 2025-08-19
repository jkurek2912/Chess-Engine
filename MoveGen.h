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
    static std::vector<Move> generateAllMoves(const Board &board);

    static U64 whiteSinglePushTargets(U64 whitePawns, U64 occupancy);
    static U64 whiteDoublePushTargets(U64 whitePawns, U64 occupancy);
    static U64 blackSinglePushTargets(U64 blackPawns, U64 occupancy);
    static U64 blackDoublePushTargets(U64 blackPawns, U64 occupancy);

    static std::vector<Move> generateWhitePawnPushes(U64 whitePawns, U64 occupancy);
    static std::vector<Move> generateBlackPawnPushes(U64 blackPawns, U64 occupancy);
};