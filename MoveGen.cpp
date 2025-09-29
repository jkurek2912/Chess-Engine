#include "Movegen.h"

void MoveGen::applyMove(Board &board, Move &move)
{
    int to = move.to;
    int from = move.from;
    Piece piece = move.piece;
    Color color = move.color;
    board.clearSquare(from);
    board.setPiece(piece, color, to);
    board.moves++;
    if (move.isCapture)
        board.movesSinceCapture = 0;
    if (move.isDoublePawnPush)
    {
        if (move.color == WHITE)
            board.enPassantSquare = from + 8;
        else
            board.enPassantSquare = from - 8;
    }
    else
    {
        board.enPassantSquare = -1;
    }
}

void MoveGen::initAttackTables()
{
    initPawnAttacks();
    initKnightAttacks();
}

uint64_t pawnAttacks[2][64]; // [color][square]
void MoveGen::initPawnAttacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t bit = 1ULL << sq;

        uint64_t whiteAttacks = 0ULL;
        if (sq % 8 != 0)
            whiteAttacks |= (bit << 7);
        if (sq % 8 != 7)
            whiteAttacks |= (bit << 9);
        pawnAttacks[WHITE][sq] = whiteAttacks;

        uint64_t blackAttacks = 0ULL;
        if (sq % 8 != 0)
            blackAttacks |= (bit >> 9);
        if (sq % 8 != 7)
            blackAttacks |= (bit >> 7);
        pawnAttacks[BLACK][sq] = blackAttacks;
    }
}

void MoveGen::initKnightAttacks()
{
}

void MoveGen::generatePawnMoves(const Board &board, std::vector<Move> &moves)
{
    generateSinglePawnPushes(board, moves);
    generateDoublePawnPushes(board, moves);
    generatePawnAttacks(board, moves);
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

void MoveGen::generatePawnAttacks(const Board &board, std::vector<Move> &moves)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    Color opColor = board.whiteToMove ? BLACK : WHITE;
    uint64_t pawns = board.pawns[color];

    while (pawns)
    {
        int from = __builtin_ctzll(pawns);
        uint64_t attacks = pawnAttacks[color][from] & board.occupancy[opColor];

        while (attacks)
        {
            int to = __builtin_ctzll(attacks);

            Move m(PAWN, color, to, from);
            m.isCapture = true;
            moves.push_back(m);

            attacks &= attacks - 1;
        }
        pawns &= pawns - 1;
    }
}