#include "Evaluation.h"
#include <bitset>

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

    int score = 0;

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

    return score;
}
