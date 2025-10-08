#include "Zobrist.h"
#include <random>

uint64_t zobristPiece[2][6][64];
uint64_t zobristCastling[4];
uint64_t zobristEnPassant[8];
uint64_t zobristSide;

static uint64_t rand64()
{
    static std::mt19937_64 rng(2025); // fixed seed for reproducibility
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    return dist(rng);
}

void initZobristKeys()
{
    for (int c = 0; c < 2; ++c)
        for (int p = 0; p < 6; ++p)
            for (int sq = 0; sq < 64; ++sq)
                zobristPiece[c][p][sq] = rand64();

    for (int i = 0; i < 4; ++i)
        zobristCastling[i] = rand64();

    for (int f = 0; f < 8; ++f)
        zobristEnPassant[f] = rand64();

    zobristSide = rand64();
}
