#include "Movegen.h"
#include <iostream>

void MoveGen::generatePseudoLegalMoves(const Board &board, std::vector<Move> &moves)
{
    generatePawnMoves(board, moves);
    generateKnightMoves(board, moves);
    generateBishopMoves(board, moves, false);
    generateRookMoves(board, moves, false);
    generateQueenMoves(board, moves);
    generateKingMoves(board, moves);
}

void MoveGen::generateLegalMoves(const Board &board, std::vector<Move> &moves)
{
    std::vector<Move> pseudoMoves;
    generatePseudoLegalMoves(board, pseudoMoves);

    for (auto &m : pseudoMoves)
    {
        Color side = m.color; // moving side
        Board copy = board;
        applyMove(copy, m);

        int kingSq = __builtin_ctzll(copy.kings[side]);
        if (!isSquareAttacked(copy, kingSq, side == WHITE ? BLACK : WHITE))
        {
            moves.push_back(m);
        }
    }
}

void MoveGen::applyMove(Board &board, Move &move)
{
    int to = move.to;
    int from = move.from;

    Piece piece = move.piece;
    Color color = move.color;
    Color enemy = (color == WHITE ? BLACK : WHITE);

    // --- captures ---
    if (move.isEnPassant)
    {
        int capSq = (color == WHITE) ? (to - 8) : (to + 8);
        board.clearSquare(PAWN, enemy, capSq);
        board.movesSinceCapture = 0;
    }
    else if (move.isCapture)
    {
        auto [capPiece, capColor] = board.findPiece(to);
        board.clearSquare(capPiece, capColor, to);
        board.movesSinceCapture = 0;
    }
    else if (piece == PAWN)
    {
        board.movesSinceCapture = 0; // pawn pushes reset the clock
    }
    else
    {
        board.movesSinceCapture++;
    }

    // --- move the piece itself ---
    board.clearSquare(piece, color, from);
    board.setPiece(piece, color, to);

    // --- en passant availability ---
    if (move.isDoublePawnPush)
    {
        board.enPassantSquare = (color == WHITE) ? (from + 8) : (from - 8);
    }
    else
    {
        board.enPassantSquare = -1;
    }

    // --- bookkeeping ---
    board.moves++;
    board.whiteToMove = !board.whiteToMove;
}
void MoveGen::initAttackTables()
{
    initPawnAttacks();
    initKnightAttacks();
    initKingAttacks();
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

uint64_t knightAttacks[64];
void MoveGen::initKnightAttacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t attacks = 0ULL;

        int row = sq / 8;
        int col = sq % 8;

        int dr[8] = {2, 2, 1, -1, -2, -2, -1, 1};
        int dc[8] = {1, -1, 2, 2, 1, -1, -2, -2};

        for (int i = 0; i < 8; i++)
        {
            int r = row + dr[i];
            int c = col + dc[i];
            if (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                int targetSq = r * 8 + c;
                attacks |= (1ULL << targetSq);
            }
        }

        knightAttacks[sq] = attacks;
    }
}

uint64_t kingAttacks[64];
void MoveGen::initKingAttacks()
{
    for (int sq = 0; sq < 64; sq++)
    {
        uint64_t bb = 0ULL;
        int row = sq / 8;
        int col = sq % 8;

        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++)
            {
                if (dr == 0 && dc == 0)
                    continue;

                int r = row + dr;
                int c = col + dc;

                if (r >= 0 && r < 8 && c >= 0 && c < 8)
                {
                    int to = r * 8 + c;
                    bb |= (1ULL << to);
                }
            }
        }
        kingAttacks[sq] = bb;
    }
}

bool MoveGen::isSquareAttacked(const Board &board, int sq, Color attacker)
{
    // --- Pawn attacks ---
    int row = sq / 8;
    int col = sq % 8;
    if (attacker == WHITE)
    {
        // white pawns attack diagonally up (toward higher sq indices)
        if (row > 0 && col > 0)
        {
            int from = sq - 9;
            if (board.pawns[WHITE] & (1ULL << from))
                return true;
        }
        if (row > 0 && col < 7)
        {
            int from = sq - 7;
            if (board.pawns[WHITE] & (1ULL << from))
                return true;
        }
    }
    else
    {
        // black pawns attack diagonally down (toward lower sq indices)
        if (row < 7 && col > 0)
        {
            int from = sq + 7;
            if (board.pawns[BLACK] & (1ULL << from))
                return true;
        }
        if (row < 7 && col < 7)
        {
            int from = sq + 9;
            if (board.pawns[BLACK] & (1ULL << from))
                return true;
        }
    }
    // --- Knights ---
    if (board.knights[attacker] & knightAttacks[sq])
        return true;

    // --- Kings ---
    if (board.kings[attacker] & kingAttacks[sq])
        return true;

    // --- Sliding pieces ---
    uint64_t occupancy = board.occupancy[BOTH];

    // rook-like (R/Q)
    const int rookDirs[4] = {8, -8, 1, -1};
    for (int d : rookDirs)
    {
        int to = sq;
        while (true)
        {
            int next = to + d;
            if (next < 0 || next >= 64)
                break;
            if ((d == 1 && to % 8 == 7) || (d == -1 && to % 8 == 0))
                break; // wrap
            to = next;
            uint64_t bb = 1ULL << to;
            if (occupancy & bb)
            {
                if (board.rooks[attacker] & bb)
                    return true;
                if (board.queens[attacker] & bb)
                    return true;
                break;
            }
        }
    }

    // bishop-like (B/Q)
    const int bishopDirs[4] = {9, 7, -7, -9};
    for (int d : bishopDirs)
    {
        int to = sq;
        while (true)
        {
            int next = to + d;
            if (next < 0 || next >= 64)
                break;
            if ((d == 9 || d == -7) && (to % 8 == 7))
                break; // wrap h-file
            if ((d == 7 || d == -9) && (to % 8 == 0))
                break; // wrap a-file
            to = next;
            uint64_t bb = 1ULL << to;
            if (occupancy & bb)
            {
                if (board.bishops[attacker] & bb)
                    return true;
                if (board.queens[attacker] & bb)
                    return true;
                break;
            }
        }
    }
    return false;
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
        Move m(PAWN, color, to, from);
        m.isDoublePawnPush = true;
        moves.emplace_back(m);

        doublePush &= doublePush - 1;
    }
}

void MoveGen::generatePawnAttacks(const Board &board, std::vector<Move> &moves)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    Color enemy = (color == WHITE) ? BLACK : WHITE;
    uint64_t pawns = board.pawns[color];

    while (pawns)
    {
        int from = __builtin_ctzll(pawns);
        uint64_t attacks = pawnAttacks[color][from];

        uint64_t captures = attacks & board.occupancy[enemy];
        while (captures)
        {
            int to = __builtin_ctzll(captures);
            Move m(PAWN, color, to, from);
            m.isCapture = true;
            moves.push_back(m);

            captures &= captures - 1;
        }

        if (board.enPassantSquare != -1 && (attacks & (1ULL << board.enPassantSquare)))
        {
            Move m(PAWN, color, board.enPassantSquare, from);
            m.isEnPassant = true;
            m.isCapture = true;
            moves.push_back(m);
        }

        pawns &= pawns - 1;
    }
}

void MoveGen::generateKnightMoves(const Board &board, std::vector<Move> &moves)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    Color opColor = (color == WHITE) ? BLACK : WHITE;
    uint64_t knights = board.knights[color];

    while (knights)
    {
        int from = __builtin_ctzll(knights);
        uint64_t attacks = knightAttacks[from];

        attacks &= ~board.occupancy[color];

        while (attacks)
        {
            int to = __builtin_ctzll(attacks);

            Move m(KNIGHT, color, to, from);
            if (board.occupancy[opColor] & (1ULL << to))
            {
                m.isCapture = true;
            }
            moves.push_back(m);

            attacks &= attacks - 1;
        }

        knights &= knights - 1;
    }
}

void MoveGen::generateBishopMoves(const Board &board, std::vector<Move> &moves, bool isQueen)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    uint64_t pieceBB = isQueen ? board.queens[color] : board.bishops[color];
    uint64_t allPieces = board.occupancy[BOTH];
    uint64_t enemyPieces = board.occupancy[(color == WHITE) ? BLACK : WHITE];

    Piece type = isQueen ? QUEEN : BISHOP;
    const int directions[4] = {9, 7, -7, -9};

    while (pieceBB)
    {
        int from = __builtin_ctzll(pieceBB);
        pieceBB &= (pieceBB - 1);

        for (int d : directions)
        {
            int to = from;

            while (true)
            {
                int next = to + d;
                if (next < 0 || next >= 64)
                    break;
                if ((d == 9 || d == -7) && (to % 8 == 7))
                    break;
                if ((d == 7 || d == -9) && (to % 8 == 0))
                    break;

                to = next;
                uint64_t toMask = 1ULL << to;

                if (allPieces & toMask)
                {
                    if (enemyPieces & toMask)
                    {
                        Move m(type, color, to, from);
                        m.isCapture = true;
                        moves.push_back(m);
                    }
                    break; // stop sliding
                }

                moves.emplace_back(type, color, to, from);
            }
        }
    }
}

void MoveGen::generateRookMoves(const Board &board, std::vector<Move> &moves, bool isQueen)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    uint64_t pieceBB = isQueen ? board.queens[color] : board.rooks[color];
    uint64_t allPieces = board.occupancy[BOTH];
    uint64_t enemyPieces = board.occupancy[(color == WHITE) ? BLACK : WHITE];

    Piece type = isQueen ? QUEEN : ROOK;
    const int directions[4] = {8, -8, 1, -1};

    while (pieceBB)
    {
        int from = __builtin_ctzll(pieceBB);
        pieceBB &= (pieceBB - 1);

        for (int d : directions)
        {
            int to = from;
            while (true)
            {
                int next = to + d;
                if (next < 0 || next >= 64)
                    break;
                if (d == 1 && (to % 8 == 7))
                    break;
                if (d == -1 && (to % 8 == 0))
                    break;

                to = next;
                uint64_t toMask = 1ULL << to;

                if (allPieces & toMask)
                {
                    if (enemyPieces & toMask)
                    {
                        Move m(type, color, to, from);
                        m.isCapture = true;
                        moves.push_back(m);
                    }
                    break;
                }

                moves.emplace_back(type, color, to, from);
            }
        }
    }
}

void MoveGen::generateQueenMoves(const Board &board, std::vector<Move> &moves)
{
    generateRookMoves(board, moves, true);
    generateBishopMoves(board, moves, true);
}

void MoveGen::generateKingMoves(const Board &board, std::vector<Move> &moves)
{
    Color color = board.whiteToMove ? WHITE : BLACK;
    Color enemy = (color == WHITE) ? BLACK : WHITE;

    uint64_t kingBB = board.kings[color];
    if (!kingBB)
        return;

    int from = __builtin_ctzll(kingBB);

    uint64_t attacks = kingAttacks[from];
    attacks &= ~board.occupancy[color];

    while (attacks)
    {
        int to = __builtin_ctzll(attacks);
        uint64_t toMask = 1ULL << to;

        Move m(KING, color, to, from);
        if (board.occupancy[enemy] & toMask)
            m.isCapture = true;

        moves.push_back(m);
        attacks &= (attacks - 1);
    }
}
