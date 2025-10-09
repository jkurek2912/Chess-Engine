#include "MoveGen.h"
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

void MoveGen::generateLegalMoves(Board &board, std::vector<Move> &moves)
{
    std::vector<Move> pseudoMoves;
    generatePseudoLegalMoves(board, pseudoMoves);
    for (auto &m : pseudoMoves)
    {
        MoveState state;
        makeMove(board, m, state);
        int kingSq = __builtin_ctzll(board.kings[m.color]);
        if (!isSquareAttacked(board, kingSq, m.color == WHITE ? BLACK : WHITE))
            moves.push_back(m);
        unmakeMove(board, m, state);
    }
}

void MoveGen::applyMove(Board &board, Move &move)
{
    int from = move.from;
    int to = move.to;
    Piece piece = move.piece;
    Color color = move.color;
    Color opp = (color == WHITE ? BLACK : WHITE);

    if (move.isEnPassant)
    {
        int capSq = (color == WHITE) ? to - 8 : to + 8;
        board.clearSquare(PAWN, opp, capSq);
        board.movesSinceCapture = 0;
    }
    else if (move.isCapture)
    {
        auto [capPiece, capColor] = board.findPiece(to);
        board.clearSquare(capPiece, capColor, to);
        board.movesSinceCapture = 0;

        if (to == 0)
            board.castlingRights[WHITEQUEEN] = false;
        if (to == 7)
            board.castlingRights[WHITEKING] = false;
        if (to == 56)
            board.castlingRights[BLACKQUEEN] = false;
        if (to == 63)
            board.castlingRights[BLACKKING] = false;
    }
    else if (piece == PAWN)
    {
        board.movesSinceCapture = 0;
    }
    else
    {
        board.movesSinceCapture++;
    }

    board.clearSquare(piece, color, from);

    if (move.isCastle)
    {
        if (color == WHITE)
        {
            if (to == 6)
            {
                board.setPiece(KING, WHITE, 6);
                board.clearSquare(ROOK, WHITE, 7);
                board.setPiece(ROOK, WHITE, 5);
            }
            else if (to == 2)
            {
                board.setPiece(KING, WHITE, 2);
                board.clearSquare(ROOK, WHITE, 0);
                board.setPiece(ROOK, WHITE, 3);
            }
            board.castlingRights[WHITEKING] = false;
            board.castlingRights[WHITEQUEEN] = false;
        }
        else
        {
            if (to == 62)
            {
                board.setPiece(KING, BLACK, 62);
                board.clearSquare(ROOK, BLACK, 63);
                board.setPiece(ROOK, BLACK, 61);
            }
            else if (to == 58)
            {
                board.setPiece(KING, BLACK, 58);
                board.clearSquare(ROOK, BLACK, 56);
                board.setPiece(ROOK, BLACK, 59);
            }
            board.castlingRights[BLACKKING] = false;
            board.castlingRights[BLACKQUEEN] = false;
        }
    }
    else if (move.isPromotion)
    {
        board.clearSquare(PAWN, color, from);
        board.setPiece(piece, color, to);
    }
    else
    {
        board.setPiece(piece, color, to);
    }

    if (piece == KING)
    {
        if (color == WHITE)
        {
            board.castlingRights[WHITEKING] = false;
            board.castlingRights[WHITEQUEEN] = false;
        }
        else
        {
            board.castlingRights[BLACKKING] = false;
            board.castlingRights[BLACKQUEEN] = false;
        }
    }
    if (piece == ROOK)
    {
        if (from == 0)
            board.castlingRights[WHITEQUEEN] = false;
        if (from == 7)
            board.castlingRights[WHITEKING] = false;
        if (from == 56)
            board.castlingRights[BLACKQUEEN] = false;
        if (from == 63)
            board.castlingRights[BLACKKING] = false;
    }

    if (move.isDoublePawnPush)
    {
        board.enPassantSquare = (color == WHITE) ? (from + 8) : (from - 8);
    }
    else
    {
        board.enPassantSquare = -1;
    }
    board.moves++;
    board.whiteToMove = !board.whiteToMove;
}

void MoveGen::makeMove(Board &board, Move &move, MoveState &state)
{
    state.castlingRights[WHITEKING] = board.castlingRights[WHITEKING];
    state.castlingRights[WHITEQUEEN] = board.castlingRights[WHITEQUEEN];
    state.castlingRights[BLACKKING] = board.castlingRights[BLACKKING];
    state.castlingRights[BLACKQUEEN] = board.castlingRights[BLACKQUEEN];
    state.enPassantSquare = board.enPassantSquare;
    state.movesSinceCapture = board.movesSinceCapture;
    state.moves = board.moves;
    state.whiteToMove = board.whiteToMove;
    state.capturedPiece = NONE;
    state.capturedColor = BOTH;
    state.capturedSquare = -1;

    Color opp = (move.color == WHITE) ? BLACK : WHITE;

    if (move.isEnPassant)
    {
        state.capturedPiece = PAWN;
        state.capturedColor = opp;
        state.capturedSquare = (move.color == WHITE) ? (move.to - 8) : (move.to + 8);
    }
    else if (move.isCapture)
    {
        auto [capPiece, capColor] = board.findPiece(move.to);
        state.capturedPiece = capPiece;
        state.capturedColor = capColor;
        state.capturedSquare = move.to;
    }
    if (board.trackRepetitions)
    {
        board.updateZobrist(move, state);
    }
    applyMove(board, move);
    if (board.trackRepetitions)
    {
        board.repetitionCount[board.hash]++;
    }
}

void MoveGen::unmakeMove(Board &board, const Move &move, const MoveState &state)
{
    int from = move.from;
    int to = move.to;
    Piece piece = move.piece;
    Color color = move.color;

    if (move.isCastle)
    {
        if (color == WHITE)
        {
            if (to == 6)
            {
                board.clearSquare(KING, WHITE, 6);
                board.setPiece(KING, WHITE, 4);
                board.clearSquare(ROOK, WHITE, 5);
                board.setPiece(ROOK, WHITE, 7);
            }
            else if (to == 2)
            {
                board.clearSquare(KING, WHITE, 2);
                board.setPiece(KING, WHITE, 4);
                board.clearSquare(ROOK, WHITE, 3);
                board.setPiece(ROOK, WHITE, 0);
            }
        }
        else
        {
            if (to == 62)
            {
                board.clearSquare(KING, BLACK, 62);
                board.setPiece(KING, BLACK, 60);
                board.clearSquare(ROOK, BLACK, 61);
                board.setPiece(ROOK, BLACK, 63);
            }
            else if (to == 58)
            {
                board.clearSquare(KING, BLACK, 58);
                board.setPiece(KING, BLACK, 60);
                board.clearSquare(ROOK, BLACK, 59);
                board.setPiece(ROOK, BLACK, 56);
            }
        }
    }
    else if (move.isPromotion)
    {
        board.clearSquare(piece, color, to);
        board.setPiece(PAWN, color, from);
    }
    else
    {
        board.clearSquare(piece, color, to);
        board.setPiece(piece, color, from);
    }

    if (state.capturedPiece != NONE)
    {
        board.setPiece(state.capturedPiece, state.capturedColor, state.capturedSquare);
    }

    board.castlingRights[WHITEKING] = state.castlingRights[WHITEKING];
    board.castlingRights[WHITEQUEEN] = state.castlingRights[WHITEQUEEN];
    board.castlingRights[BLACKKING] = state.castlingRights[BLACKKING];
    board.castlingRights[BLACKQUEEN] = state.castlingRights[BLACKQUEEN];
    board.enPassantSquare = state.enPassantSquare;
    board.movesSinceCapture = state.movesSinceCapture;
    board.moves = state.moves;
    board.whiteToMove = state.whiteToMove;
    if (board.trackRepetitions)
    {
        board.updateZobrist(move, state);
        board.repetitionCount[board.hash]--;
        if (board.repetitionCount[board.hash] == 0)
            board.repetitionCount.erase(board.hash);
    }
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

// is the [color] king in check?
bool MoveGen::inCheck(const Board &board, Color color)
{
    int kingSq = __builtin_ctzll(board.kings[color]);
    return isSquareAttacked(board, kingSq, color == WHITE ? BLACK : WHITE);
}

bool MoveGen::isSquareAttacked(const Board &board, int sq, Color attacker)
{
    int row = sq / 8;
    int col = sq % 8;
    if (attacker == WHITE)
    {
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
    if (board.knights[attacker] & knightAttacks[sq])
        return true;

    if (board.kings[attacker] & kingAttacks[sq])
        return true;

    uint64_t occupancy = board.occupancy[BOTH];

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
                break;
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
                break;
            if ((d == 7 || d == -9) && (to % 8 == 0))
                break;
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
                    break;
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
        {
            m.isCapture = true;
        }
        moves.push_back(m);

        attacks &= (attacks - 1);
    }

    if (color == WHITE)
    {
        if (board.castlingRights[WHITEKING])
        {
            if (!(board.occupancy[BOTH] & ((1ULL << 5) | (1ULL << 6))))
            {
                if (!isSquareAttacked(board, 4, enemy) &&
                    !isSquareAttacked(board, 5, enemy) &&
                    !isSquareAttacked(board, 6, enemy))
                {
                    Move m(KING, WHITE, 6, 4);
                    m.isCastle = true;
                    moves.push_back(m);
                }
            }
        }
        if (board.castlingRights[WHITEQUEEN])
        {
            if (!(board.occupancy[BOTH] & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))))
            {
                if (!isSquareAttacked(board, 4, enemy) &&
                    !isSquareAttacked(board, 3, enemy) &&
                    !isSquareAttacked(board, 2, enemy))
                {
                    Move m(KING, WHITE, 2, 4);
                    m.isCastle = true;
                    moves.push_back(m);
                }
            }
        }
    }
    else
    {
        if (board.castlingRights[BLACKKING])
        {
            if (!(board.occupancy[BOTH] & ((1ULL << 61) | (1ULL << 62))))
            {
                if (!isSquareAttacked(board, 60, enemy) &&
                    !isSquareAttacked(board, 61, enemy) &&
                    !isSquareAttacked(board, 62, enemy))
                {
                    Move m(KING, BLACK, 62, 60);
                    m.isCastle = true;
                    moves.push_back(m);
                }
            }
        }
        if (board.castlingRights[BLACKQUEEN])
        {
            if (!(board.occupancy[BOTH] & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))))
            {
                if (!isSquareAttacked(board, 60, enemy) &&
                    !isSquareAttacked(board, 59, enemy) &&
                    !isSquareAttacked(board, 58, enemy))
                {
                    Move m(KING, BLACK, 58, 60);
                    m.isCastle = true;
                    moves.push_back(m);
                }
            }
        }
    }
}

void MoveGen::printAttackMap(const Board &board, Color attacker)
{
    std::cout << "Attack map for " << (attacker == WHITE ? "WHITE" : "BLACK") << ":\n";
    for (int r = 7; r >= 0; r--)
    {
        for (int c = 0; c < 8; c++)
        {
            int sq = r * 8 + c;
            if (isSquareAttacked(board, sq, attacker))
            {
                std::cout << "x ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}