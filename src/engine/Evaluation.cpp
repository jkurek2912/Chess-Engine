#include "Evaluation.h"
#include <bitset>
#include <iostream>

inline int mirror(int sq) { return sq ^ 56; } // flips rank (A1 ↔ A8)

int evaluate(const Board &b)
{
    static const int pieceValues[6] = {
        100,  // PAWN
        320,  // KNIGHT
        330,  // BISHOP
        500,  // ROOK
        900,  // QUEEN
        20000 // KING
    };

    static const int pawnPST[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0};

    static const int knightPST[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50};

    static const int bishopPST[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20};

    static const int rookPST[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0};

    static const int queenPST[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20};

    int score = 0;

    // --- Material evaluation ---
    score += __builtin_popcountll(b.pawns[WHITE]) * pieceValues[PAWN];
    score += __builtin_popcountll(b.knights[WHITE]) * pieceValues[KNIGHT];
    score += __builtin_popcountll(b.bishops[WHITE]) * pieceValues[BISHOP];
    score += __builtin_popcountll(b.rooks[WHITE]) * pieceValues[ROOK];
    score += __builtin_popcountll(b.queens[WHITE]) * pieceValues[QUEEN];

    score -= __builtin_popcountll(b.pawns[BLACK]) * pieceValues[PAWN];
    score -= __builtin_popcountll(b.knights[BLACK]) * pieceValues[KNIGHT];
    score -= __builtin_popcountll(b.bishops[BLACK]) * pieceValues[BISHOP];
    score -= __builtin_popcountll(b.rooks[BLACK]) * pieceValues[ROOK];
    score -= __builtin_popcountll(b.queens[BLACK]) * pieceValues[QUEEN];

    // --- Positional bonuses via PSTs ---

    // Pawns
    uint64_t wpawns = b.pawns[WHITE];
    while (wpawns)
    {
        int sq = __builtin_ctzll(wpawns);
        score += pawnPST[sq];
        wpawns &= wpawns - 1;
    }

    uint64_t bpawns = b.pawns[BLACK];
    while (bpawns)
    {
        int sq = __builtin_ctzll(bpawns);
        score -= pawnPST[mirror(sq)];
        bpawns &= bpawns - 1;
    }

    // Knights
    uint64_t wknights = b.knights[WHITE];
    while (wknights)
    {
        int sq = __builtin_ctzll(wknights);
        score += knightPST[sq];
        wknights &= wknights - 1;
    }

    uint64_t bknights = b.knights[BLACK];
    while (bknights)
    {
        int sq = __builtin_ctzll(bknights);
        score -= knightPST[mirror(sq)];
        bknights &= bknights - 1;
    }

    // Bishops
    uint64_t wbishops = b.bishops[WHITE];
    while (wbishops)
    {
        int sq = __builtin_ctzll(wbishops);
        score += bishopPST[sq];
        wbishops &= wbishops - 1;
    }

    uint64_t bbishops = b.bishops[BLACK];
    while (bbishops)
    {
        int sq = __builtin_ctzll(bbishops);
        score -= bishopPST[mirror(sq)];
        bbishops &= bbishops - 1;
    }

    // Rooks
    uint64_t wrooks = b.rooks[WHITE];
    while (wrooks)
    {
        int sq = __builtin_ctzll(wrooks);
        score += rookPST[sq];
        wrooks &= wrooks - 1;
    }

    uint64_t brooks = b.rooks[BLACK];
    while (brooks)
    {
        int sq = __builtin_ctzll(brooks);
        score -= rookPST[mirror(sq)];
        brooks &= brooks - 1;
    }

    // Queens
    uint64_t wqueens = b.queens[WHITE];
    while (wqueens)
    {
        int sq = __builtin_ctzll(wqueens);
        score += queenPST[sq];
        wqueens &= wqueens - 1;
    }

    uint64_t bqueens = b.queens[BLACK];
    while (bqueens)
    {
        int sq = __builtin_ctzll(bqueens);
        score -= queenPST[mirror(sq)];
        bqueens &= bqueens - 1;
    }

    return score;
}
