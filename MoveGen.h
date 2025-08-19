#pragma once
#include "Board.h"
#include <vector>

#define northOne(b) ((b) << 8)
#define southOne(b) ((b) >> 8)
#define eastOne(b)  (((b) & 0x7f7f7f7f7f7f7f7fULL) << 1) // not H file
#define westOne(b)  (((b) & 0xfefefefefefefefeULL) >> 1) // not A file


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
    static void initAttackTables();
    static void initPawnAttacks();

    static void initKnightAttacks();

    static void initKingMoves();

    static std::vector<Move> generateAllMoves(const Board &board);

    static std::vector<Move> generateWhitePawnPushes(U64 whitePawns, U64 occupancy);
    static std::vector<Move> generateBlackPawnPushes(U64 blackPawns, U64 occupancy);

    static std::vector<Move> generateWhitePawnAttacks(U64 whitePawns, U64 blackOccupancy);
    static std::vector<Move> generateBlackPawnAttacks(U64 blackPawns, U64 whiteOccupancy);

    static std::vector<Move> generateWhiteKnightMoves(U64 whiteKnights, U64 whiteOccupancy);
    static std::vector<Move> generateBlackKnightMoves(U64 blackKnights, U64 blackOccupancy);

    static std::vector<Move> generateWhiteBishopMoves(U64 whiteBishops, U64 whiteOccupancy, U64 blackOccupancy);
    static std::vector<Move> generateBlackBishopMoves(U64 blackBishops, U64 blackOccupancy, U64 whiteOccupancy);

    
    static std::vector<Move> generateWhiteKingMoves(U64 whiteKing, U64 whiteOccupancy);
    static std::vector<Move> generateBlackKingMoves(U64 blackKing, U64 blackOccupancy);

private:
    static U64 arrPawnAttacks[2][64];
    static U64 arrKnightMoves[64];
    static U64 arrKingMoves[64];
};