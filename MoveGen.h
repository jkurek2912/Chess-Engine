#pragma once
#include "Board.h"
#include <vector>

inline U64 squareBB(int sq) { return 1ULL << sq; }
constexpr U64 notAFile = 0xfefefefefefefefeULL;
constexpr U64 notHFile = 0x7f7f7f7f7f7f7f7fULL;
constexpr U64 notGHFile = 0x3F3F3F3F3F3F3F3FULL;
constexpr U64 notABFile = 0xFCFCFCFCFCFCFCFCULL;

inline U64 northOne(U64 b) { return b << 8; }
inline U64 southOne(U64 b) { return b >> 8; }
inline U64 eastOne(U64 b) { return (b & 0x7f7f7f7f7f7f7f7fULL) << 1; }
inline U64 westOne(U64 b) { return (b & 0xfefefefefefefefeULL) >> 1; }

inline U64 shiftNE(U64 b) { return (b & notHFile) << 9; }
inline U64 shiftSE(U64 b) { return (b & notHFile) >> 7; }
inline U64 shiftSW(U64 b) { return (b & notAFile) >> 9; }
inline U64 shiftNW(U64 b) { return (b & notAFile) << 7; }

inline U64 shiftNNE(U64 b) { return (b & notHFile) << 17; }
inline U64 shiftNEE(U64 b) { return (b & notGHFile) << 10; }
inline U64 shiftSEE(U64 b) { return (b & notGHFile) >> 6; }
inline U64 shiftSSE(U64 b) { return (b & notHFile) >> 15; }

inline U64 shiftNNW(U64 b) { return (b & notAFile) << 15; }
inline U64 shiftNWW(U64 b) { return (b & notABFile) << 6; }
inline U64 shiftSWW(U64 b) { return (b & notABFile) >> 10; }
inline U64 shiftSSW(U64 b) { return (b & notAFile) >> 17; }

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

    static std::vector<Move> generatePawnPushes(U64 pawns, U64 occupancy, COLOR color);

    static std::vector<Move> generatePawnAttacks(U64 pawns, U64 occupancy, COLOR color);

    static std::vector<Move> generateKnightMoves(U64 knights, U64 occupancy);

    static std::vector<Move> generateBishopMoves(U64 bishops, U64 occupancy, U64 friendlyPieces);

    static std::vector<Move> generateRookMoves(U64 rooks, U64 occupancy, U64 friendlyPieces);

    static std::vector<Move> generateQueenMoves(U64 queens, U64 occupancy, U64 friendlyPieces);

    static std::vector<Move> generateKingMoves(U64 king, U64 occupancy);

private:
    static U64 arrPawnAttacks[2][64];
    static U64 arrKnightMoves[64];
    static U64 arrKingMoves[64];
};