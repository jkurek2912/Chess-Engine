#include "Movegen.h"

void clearSquare(Board &board, Piece piece, Color color, int square)
{
    uint64_t mask = ~(1ULL << square);

    switch (piece)
    {
    case PAWN:
        board.pawns[color] &= mask;
        break;
    case KNIGHT:
        board.knights[color] &= mask;
        break;
    case BISHOP:
        board.bishops[color] &= mask;
        break;
    case ROOK:
        board.rooks[color] &= mask;
        break;
    case QUEEN:
        board.queens[color] &= mask;
        break;
    case KING:
        board.kings[color] &= mask;
        break;
    }

    board.occupancy[color] &= mask;
    board.occupancy[BOTH] &= mask;
}
void applyMove(Board &board, const Move &move)
{
}

void MoveGen::generatePawnMoves(const Board &board, std::vector<Move> &moves)
{
    generateSinglePawnPushes(board, moves);
    generateDoublePawnPushes(board, moves);
}

void MoveGen::generateSinglePawnPushes(const Board &board, std::vector<Move> &moves)
{
    uint64_t singlePush = 0;
    Color color = board.whiteToMove ? WHITE : BLACK;
    if (color == WHITE)
    {
        singlePush = (board.pawns[WHITE] << 8) & (~board.occupancy[BOTH]);
    }
    else
    {
        singlePush = (board.pawns[BLACK] >> 8) & (~board.occupancy[BOTH]);
    }
    int dif = (board.whiteToMove) ? 8 : -8;
    while (singlePush)
    {
        int to = __builtin_ctzll(singlePush);
        int from = to - dif;

        moves.emplace_back(PAWN, color, to, from);

        singlePush &= singlePush - 1;
    }
}

void MoveGen::generateDoublePawnPushes(const Board &board, std::vector<Move> &moves)
{
    uint64_t doublePush = 0;
    Color color = board.whiteToMove ? WHITE : BLACK;
    if (color == WHITE)
    {
        uint64_t pawnsOnRank2 = board.pawns[WHITE] & 0x000000000000FF00ULL;
        doublePush = (pawnsOnRank2 << 16) & (~board.occupancy[BOTH]) & (~board.occupancy[BOTH] << 8);
    }
    else
    {
        uint64_t pawnsOnRank7 = board.pawns[BLACK] & 0x00FF000000000000ULL;
        doublePush = (pawnsOnRank7 >> 16) & (~board.occupancy[BOTH]) & (~board.occupancy[BOTH] >> 8);
    }
    int dif = (board.whiteToMove) ? 16 : -16;
    while (doublePush)
    {
        int to = __builtin_ctzll(doublePush);
        int from = to - dif;

        moves.emplace_back(PAWN, color, to, from);

        doublePush &= doublePush - 1;
    }
}