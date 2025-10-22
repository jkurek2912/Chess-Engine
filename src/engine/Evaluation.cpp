#include "Evaluation.h"
#include "MoveGen.h"
#include <bitset>
#include <iostream>

inline int mirror(int sq) { return sq ^ 56; } // flips rank (A1 ↔ A8)

static const int pieceValues[6] = {
    100,  // PAWN
    320,  // KNIGHT
    330,  // BISHOP
    500,  // ROOK
    900,  // QUEEN
    20000 // KING
};

static const int whitePawnPST[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 5, 5, 0, 0, 0,
    5, 5, 10, 20, 20, 5, 5, 5,
    10, 10, 10, 20, 20, 10, 10, 10,
    20, 20, 20, 30, 30, 30, 20, 20,
    30, 30, 30, 40, 40, 30, 30, 30,
    90, 90, 90, 90, 90, 90, 90, 90};

static const int blackPawnPST[64] = {
    90, 90, 90, 90, 90, 90, 90, 90,
    30, 30, 30, 40, 40, 30, 30, 30,
    20, 20, 20, 30, 30, 30, 20, 20,
    10, 10, 10, 20, 20, 10, 10, 10,
    5, 5, 10, 20, 20, 5, 5, 5,
    0, 0, 0, 5, 5, 0, 0, 0,
    0, 0, 0, -10, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

static const int knightPST[64] = {
    -50, -29, -30, -30, -30, -30, -29, -50,
    -40, -20, 0, 0, 0, 0, -20, -40,
    -30, 0, 10, 15, 15, 10, 0, -30,
    -30, 5, 15, 20, 20, 15, 5, -30,
    -30, 0, 15, 20, 20, 15, 0, -30,
    -30, 5, 10, 15, 15, 10, 5, -30,
    -40, -20, 0, 5, 5, 0, -20, -40,
    -50, -29, -30, -30, -30, -30, -29, -50};

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

static const int WHITE_KINGPST[64] = {
    20, 30, 10, 0, 0, 10, 30, 20,
    20, 20, 0, 0, 0, 0, 20, 20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30};

static const int BLACK_KINGPSt[64] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    20, 20, 0, 0, 0, 0, 20, 20,
    20, 30, 10, 0, 0, 10, 30, 20};

static const int WHITE_KINGEndgame[64] = {
    -50, -30, -30, -30, -30, -30, -30, -50,
    -30, -30, 0, 0, 0, 0, -30, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -50, -40, -30, -20, -20, -30, -40, -50};

static const int BLACK_KINGEndgame[64] = {
    -50, -40, -30, -20, -20, -30, -40, -50,
    -30, -20, -10, 0, 0, -10, -20, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 30, 40, 40, 30, -10, -30,
    -30, -10, 20, 30, 30, 20, -10, -30,
    -30, -30, 0, 0, 0, 0, -30, -30,
    -50, -30, -30, -30, -30, -30, -30, -50};

int evaluate(Board &b)
{
    int scoreWhite = 0;
    int scoreBlack = 0;

    scoreWhite += __builtin_popcountll(b.pawns[WHITE]) * pieceValues[PAWN];
    scoreWhite += __builtin_popcountll(b.knights[WHITE]) * pieceValues[KNIGHT];
    scoreWhite += __builtin_popcountll(b.bishops[WHITE]) * pieceValues[BISHOP];
    scoreWhite += __builtin_popcountll(b.rooks[WHITE]) * pieceValues[ROOK];
    scoreWhite += __builtin_popcountll(b.queens[WHITE]) * pieceValues[QUEEN];

    scoreBlack += __builtin_popcountll(b.pawns[BLACK]) * pieceValues[PAWN];
    scoreBlack += __builtin_popcountll(b.knights[BLACK]) * pieceValues[KNIGHT];
    scoreBlack += __builtin_popcountll(b.bishops[BLACK]) * pieceValues[BISHOP];
    scoreBlack += __builtin_popcountll(b.rooks[BLACK]) * pieceValues[ROOK];
    scoreBlack += __builtin_popcountll(b.queens[BLACK]) * pieceValues[QUEEN];

    int totalScore = scoreWhite + scoreBlack;
    bool endGame = (totalScore <= 2400);
    int score = scoreWhite - scoreBlack;

    uint64_t wpawns = b.pawns[WHITE];
    while (wpawns)
    {
        int sq = __builtin_ctzll(wpawns);
        score += whitePawnPST[sq];
        wpawns &= wpawns - 1;
    }

    uint64_t bpawns = b.pawns[BLACK];
    while (bpawns)
    {
        int sq = __builtin_ctzll(bpawns);
        score -= blackPawnPST[sq];
        bpawns &= bpawns - 1;
    }

    uint64_t wknights = b.knights[WHITE];
    while (wknights)
    {
        int sq = __builtin_ctzll(wknights);
        score += knightPST[mirror(sq)];
        wknights &= wknights - 1;
    }

    uint64_t bknights = b.knights[BLACK];
    while (bknights)
    {
        int sq = __builtin_ctzll(bknights);
        score -= knightPST[sq];
        bknights &= bknights - 1;
    }

    uint64_t wbishops = b.bishops[WHITE];
    while (wbishops)
    {
        int sq = __builtin_ctzll(wbishops);
        score += bishopPST[mirror(sq)];
        wbishops &= wbishops - 1;
    }

    uint64_t bbishops = b.bishops[BLACK];
    while (bbishops)
    {
        int sq = __builtin_ctzll(bbishops);
        score -= bishopPST[sq];
        bbishops &= bbishops - 1;
    }

    uint64_t wrooks = b.rooks[WHITE];
    while (wrooks)
    {
        int sq = __builtin_ctzll(wrooks);
        score += rookPST[mirror(sq)];
        wrooks &= wrooks - 1;
    }

    uint64_t brooks = b.rooks[BLACK];
    while (brooks)
    {
        int sq = __builtin_ctzll(brooks);
        score -= rookPST[sq];
        brooks &= brooks - 1;
    }

    uint64_t wqueens = b.queens[WHITE];
    while (wqueens)
    {
        int sq = __builtin_ctzll(wqueens);
        score += queenPST[mirror(sq)];
        wqueens &= wqueens - 1;
    }

    uint64_t bqueens = b.queens[BLACK];
    while (bqueens)
    {
        int sq = __builtin_ctzll(bqueens);
        score -= queenPST[sq];
        bqueens &= bqueens - 1;
    }

    uint64_t wking = b.kings[WHITE];
    if (wking)
    {
        int sq = __builtin_ctzll(wking);
        if (endGame)
            score += WHITE_KINGEndgame[sq];
        else
            score += WHITE_KINGPST[sq];
    }

    uint64_t bking = b.kings[BLACK];
    if (bking)
    {
        int sq = __builtin_ctzll(bking);
        if (endGame)
            score -= BLACK_KINGEndgame[sq];
        else
            score -= BLACK_KINGPSt[sq];
    }
    return b.whiteToMove ? score : -score;
}
