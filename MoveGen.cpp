#include "MoveGen.h"
#include "Board.h"

std::vector<Move> MoveGen::generatePseudoLegalMoves(const Board &board, COLOR color)
{
    std::vector<Move> moves;

    U64 friendly = (color == WHITE) ? board.getOccupancy(WHITE) : board.getOccupancy(BLACK);
    U64 enemy = (color == WHITE) ? board.getOccupancy(BLACK) : board.getOccupancy(WHITE);
    U64 both = board.getOccupancy(BOTH);

    U64 pawns = (color == WHITE) ? board.getPawns(WHITE) : board.getPawns(BLACK);
    U64 knights = (color == WHITE) ? board.getKnights(WHITE) : board.getKnights(BLACK);
    U64 bishops = (color == WHITE) ? board.getBishops(WHITE) : board.getBishops(BLACK);
    U64 rooks = (color == WHITE) ? board.getRooks(WHITE) : board.getRooks(BLACK);
    U64 queens = (color == WHITE) ? board.getQueens(WHITE) : board.getQueens(BLACK);
    U64 king = (color == WHITE) ? board.getKings(WHITE) : board.getKings(BLACK);

    std::vector<Move> pawnPushes = generatePawnPushes(pawns, both, color);
    std::vector<Move> pawnAttacks = generatePawnAttacks(pawns, enemy, color, board.getEnPassantSquare());

    std::vector<Move> knightMoves = generateKnightMoves(knights, friendly);
    std::vector<Move> bishopMoves = generateBishopMoves(bishops, friendly, enemy);
    std::vector<Move> rookMoves = generateRookMoves(rooks, friendly, enemy);
    std::vector<Move> queenMoves = generateQueenMoves(queens, friendly, enemy);
    std::vector<Move> kingMoves = generateKingMoves(king, friendly, board);

    moves.insert(moves.end(), pawnPushes.begin(), pawnPushes.end());
    moves.insert(moves.end(), pawnAttacks.begin(), pawnAttacks.end());
    moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), queenMoves.begin(), queenMoves.end());
    moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());

    return moves;
}

std::vector<Move> MoveGen::generateAllLegalMoves(const Board &board, COLOR color)
{
    std::vector<Move> legalMoves;
    std::vector<Move> moves = generatePseudoLegalMoves(board, color);

    for (auto move : moves)
    {
        Board copy = board;
        int from = move.from;
        int to = move.to;
        PIECE piece = move.piece;
        if (move.isCastling())
        {
            int kingFrom = (color == WHITE) ? 4 : 60;
            int kingTo = move.kingsideCastle ? kingFrom + 2 : kingFrom - 2;
            int rookFrom = move.kingsideCastle ? ((color == WHITE) ? 7 : 63) : ((color == WHITE) ? 0 : 56);
            int rookTo = move.kingsideCastle ? kingFrom + 1 : kingFrom - 1;

            copy.clearSquare(kingFrom);
            copy.setPiece(KING, color, kingTo);

            copy.clearSquare(rookFrom);
            copy.setPiece(ROOK, color, rookTo);
        }
        else
        {
            copy.clearSquare(from);

            if (move.isEnPassant)
            {
                copy.setPiece(piece, color, to);

                int capturedSq = (color == WHITE) ? to - 8 : to + 8;
                copy.clearSquare(capturedSq);
            }
            else
            {
                copy.clearSquare(to);
                copy.setPiece(piece, color, to);
            }
        }

        if (!kingInCheck(copy, color))
            legalMoves.push_back(move);
    }
    return legalMoves;
}

bool MoveGen::kingInCheck(const Board &board, COLOR color)
{
    Board copy = board;

    int kingSq = __builtin_ctzll(copy.getKings(color));

    COLOR enemy = (color == WHITE) ? BLACK : WHITE;

    U64 enemyPawns = copy.getPawns(enemy);
    U64 pawns = enemyPawns;

    while (pawns)
    {
        int sq = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        if (arrPawnAttacks[enemy][sq] & (1ULL << kingSq))
        {
            return true;
        }
    }

    U64 enemyKnights = copy.getKnights(enemy);
    if (MoveGen::arrKnightMoves[kingSq] & enemyKnights)
    {
        return true;
    }

    U64 enemyBishops = copy.getBishops(enemy);
    U64 enemyQueens = copy.getQueens(enemy);
    U64 diagAttackers = enemyBishops | enemyQueens;
    U64 bishopAttacks = generateDiagAttackBitboard(kingSq, copy.getOccupancy(BOTH));
    if (bishopAttacks & diagAttackers)
    {
        return true;
    }

    U64 enemyRooks = copy.getRooks(enemy);
    U64 straightAttackers = enemyRooks | enemyQueens;
    U64 rookAttacks = generateStraightLineAttackBitboard(kingSq, copy.getOccupancy(BOTH));
    if (rookAttacks & straightAttackers)
    {
        return true;
    }

    U64 enemyKing = copy.getKings(enemy);
    if (MoveGen::arrKingMoves[kingSq] & enemyKing)
        return true;

    return false;
}

inline U64 MoveGen::generateDiagAttackBitboard(int sq, U64 occupancy)
{
    U64 attacks = 0ULL;
    int file = sq % 8, rank = sq / 8;

    // NE
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++)
    {
        int target = r * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break; // blocked
    }
    // NW
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--)
    {
        int target = r * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }
    // SE
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++)
    {
        int target = r * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }
    // SW
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
    {
        int target = r * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }

    return attacks;
}

inline U64 MoveGen::generateStraightLineAttackBitboard(int sq, U64 occupancy)
{
    U64 attacks = 0ULL;
    int file = sq % 8, rank = sq / 8;

    // North
    for (int r = rank + 1; r < 8; r++)
    {
        int target = r * 8 + file;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }
    // South
    for (int r = rank - 1; r >= 0; r--)
    {
        int target = r * 8 + file;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }
    // East
    for (int f = file + 1; f < 8; f++)
    {
        int target = rank * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }
    // West
    for (int f = file - 1; f >= 0; f--)
    {
        int target = rank * 8 + f;
        attacks |= (1ULL << target);
        if (occupancy & (1ULL << target))
            break;
    }

    return attacks;
}

void MoveGen::initAttackTables()
{
    initPawnAttacks();
    initKnightAttacks();
    initKingMoves();
}

// pawn move gen
U64 MoveGen::arrPawnAttacks[2][64];

void MoveGen::initPawnAttacks()
{
    for (int sq = 0; sq < TOTAL_SQUARES; sq++)
    {
        U64 bb = squareBB(sq);
        arrPawnAttacks[WHITE][sq] = shiftNE(bb) | shiftNW(bb);
        arrPawnAttacks[BLACK][sq] = shiftSE(bb) | shiftSW(bb);
    }
}

// knight move gen
U64 MoveGen::arrKnightMoves[64];

void MoveGen::initKnightAttacks()
{
    for (int sq = 0; sq < TOTAL_SQUARES; sq++)
    {
        U64 bb = squareBB(sq);
        U64 attacks = 0ULL;

        attacks |= shiftNNE(bb);
        attacks |= shiftNEE(bb);
        attacks |= shiftSEE(bb);
        attacks |= shiftSSE(bb);
        attacks |= shiftNNW(bb);
        attacks |= shiftNWW(bb);
        attacks |= shiftSWW(bb);
        attacks |= shiftSSW(bb);

        arrKnightMoves[sq] = attacks;
    }
}

// king move gen
U64 MoveGen::arrKingMoves[64];

U64 kingAttacks(U64 kingSet)
{
    U64 attacks = eastOne(kingSet) | westOne(kingSet);
    kingSet |= attacks;
    attacks |= northOne(kingSet) | southOne(kingSet);
    return attacks;
}

void MoveGen::initKingMoves()
{
    U64 sqBB = 1;
    for (int sq = 0; sq < 64; sq++, sqBB <<= 1)
        arrKingMoves[sq] = kingAttacks(sqBB);
}

inline void addMovesFromBitboard(std::vector<Move> &moves, int from, U64 attacks, PIECE piece)
{
    while (attacks)
    {
        int to = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        moves.push_back({from, to, piece});
    }
}

U64 whiteSinglePushTargets(U64 whitePawns, U64 occupancy)
{
    return northOne(whitePawns) & ~occupancy;
}

U64 whiteDoublePushTargets(U64 whitePawns, U64 occupancy)
{
    const U64 rank4 = 0x00000000FF000000ULL;
    U64 singlePushs = whiteSinglePushTargets(whitePawns, occupancy);
    return northOne(singlePushs) & ~occupancy & rank4;
}

U64 blackSinglePushTargets(U64 blackPawns, U64 occupancy)
{
    return southOne(blackPawns) & ~occupancy;
}

U64 blackDoublePushTargets(U64 blackPawns, U64 occupancy)
{
    const U64 rank5 = 0x000000FF00000000ULL;
    U64 singlePushs = blackSinglePushTargets(blackPawns, occupancy);
    return southOne(singlePushs) & ~occupancy & rank5;
}

std::vector<Move> MoveGen::generatePawnPushes(U64 pawns, U64 occupancy, COLOR color)
{
    std::vector<Move> moves;
    U64 pushTargets = (color == WHITE) ? whiteSinglePushTargets(pawns, occupancy) : blackSinglePushTargets(pawns, occupancy);

    while (pushTargets)
    {
        int toSquare = __builtin_ctzll(pushTargets);
        pushTargets &= pushTargets - 1;

        int fromSquare = (color == WHITE) ? toSquare - 8 : toSquare + 8;

        bool promotion = (color == WHITE) ? (toSquare / 8 == 7) : (toSquare / 8 == 0);

        if (promotion)
        {
            moves.push_back({fromSquare, toSquare, QUEEN});
        }
        else
        {
            moves.push_back({fromSquare, toSquare, PAWN});
        }
    }

    pushTargets = (color == WHITE) ? whiteDoublePushTargets(pawns, occupancy) : blackDoublePushTargets(pawns, occupancy);

    while (pushTargets)
    {
        int toSquare = __builtin_ctzll(pushTargets);
        pushTargets &= pushTargets - 1;

        int fromSquare = (color == WHITE) ? toSquare - 16 : toSquare + 16;
        moves.push_back({fromSquare, toSquare, PAWN});
    }
    return moves;
}

std::vector<Move> MoveGen::generatePawnAttacks(U64 pawns, U64 enemy, COLOR color, int enPassantSquare)
{
    std::vector<Move> moves;

    while (pawns)
    {
        int from = __builtin_ctzll(pawns);
        pawns &= pawns - 1;

        U64 attacks = (color == WHITE)
            ? MoveGen::arrPawnAttacks[WHITE][from] & enemy
            : MoveGen::arrPawnAttacks[BLACK][from] & enemy;

        while (attacks)
        {
            int to = __builtin_ctzll(attacks);
            attacks &= attacks - 1;

            bool promotion = (color == WHITE) ? (to / 8 == 7) : (to / 8 == 0);
            moves.push_back({from, to, promotion ? QUEEN : PAWN});
        }

        if (enPassantSquare != -1)
        {
            U64 epMask = 1ULL << enPassantSquare;
            U64 epAttacks = (color == WHITE)
                ? MoveGen::arrPawnAttacks[WHITE][from]
                : MoveGen::arrPawnAttacks[BLACK][from];

            if (epAttacks & epMask)
            {
                Move epMove;
                epMove.from = from;
                epMove.to = enPassantSquare;
                epMove.piece = PAWN;
                epMove.isEnPassant = true;
                moves.push_back(epMove);
            }
        }
    }

    return moves;
}

std::vector<Move> MoveGen::generateKnightMoves(U64 knights, U64 friendly)
{
    std::vector<Move> moves;

    while (knights)
    {
        int sq = __builtin_ctzll(knights);
        knights &= knights - 1;

        U64 attacks = MoveGen::arrKnightMoves[sq] & ~friendly;

        addMovesFromBitboard(moves, sq, attacks, KNIGHT);
    }
    return moves;
}

bool squareAttacked(int sq, const Board &board, COLOR attacker)
{
    auto moves = MoveGen::generatePseudoLegalMoves(board, attacker);
    for (auto &m : moves)
    {
        if (m.to == sq)
            return true;
    }
    return false;
}

std::vector<Move> MoveGen::generateKingMoves(U64 king, U64 friendly, const Board &board)
{
    std::vector<Move> moves;
    int from = __builtin_ctzll(king);
    COLOR color = (king & board.getKings(WHITE)) ? WHITE : BLACK;

    U64 attacks = MoveGen::arrKingMoves[from] & ~friendly;
    while (attacks)
    {
        int to = __builtin_ctzll(attacks);
        attacks &= attacks - 1;
        moves.push_back(Move(from, to, KING));
    }

    std::array<bool, 4> castlingRights = board.getCastlingRights();

    bool kingsideCastle = (color == WHITE) ? castlingRights[0] : castlingRights[2];
    bool queensideCastle = (color == WHITE) ? castlingRights[1] : castlingRights[3];
    if (!kingInCheck(board, color))
    {
        U64 occupancy = board.getOccupancy(BOTH);

        if (color == WHITE)
        {
            if (kingsideCastle &&
                    !(occupancy & ((1ULL << 5) | (1ULL << 6))) &&
                    !squareAttacked(5, board, BLACK) && // f1 not attacked
                    !squareAttacked(6, board, BLACK))   // g1 not attacked
            {
                moves.push_back(Move(true, false));
            }

            if (queensideCastle &&
                    !(occupancy & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) &&
                    !squareAttacked(3, board, BLACK) && // d1 not attacked
                    !squareAttacked(2, board, BLACK))   // c1 not attacked
            {
                moves.push_back(Move(false, true));
            }
        }
        else
        {
            if (kingsideCastle &&
                    !(occupancy & ((1ULL << 61) | (1ULL << 62))) &&
                    !squareAttacked(61, board, WHITE) && // f8 not attacked
                    !squareAttacked(62, board, WHITE))   // g8 not attacked
            {
                moves.push_back(Move(true, false));
            }

            if (queensideCastle &&
                    !(occupancy & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) &&
                    !squareAttacked(59, board, WHITE) && // d8 not attacked
                    !squareAttacked(58, board, WHITE))   // c8 not attacked
            {
                moves.push_back(Move(false, true));
            }
        }
    }

    return moves;
}

bool MoveGen::squaresAttacked(const Board &board, COLOR color, const std::vector<int> &squares)
{
    COLOR enemy = (color == WHITE) ? BLACK : WHITE;

    for (int sq : squares)
    {
        U64 enemyPawns = board.getPawns(enemy);
        if ((MoveGen::arrPawnAttacks[color][sq] & enemyPawns) != 0)
            return true;

        U64 enemyKnights = board.getKnights(enemy);
        if ((MoveGen::arrKnightMoves[sq] & enemyKnights) != 0)
            return true;

        U64 diagAttackers = board.getBishops(enemy) | board.getQueens(enemy);
        if (generateDiagAttackBitboard(sq, board.getOccupancy(BOTH)) & diagAttackers)
            return true;

        U64 straightAttackers = board.getRooks(enemy) | board.getQueens(enemy);
        if (generateStraightLineAttackBitboard(sq, board.getOccupancy(BOTH)) & straightAttackers)
            return true;

        U64 enemyKing = board.getKings(enemy);
        if ((MoveGen::arrKingMoves[sq] & enemyKing) != 0)
            return true;
    }

    return false;
}

std::vector<Move> bishopMovesFromSquare(int sq, U64 friendly, U64 enemy)
{
    std::vector<Move> moves;
    int r = sq / 8;
    int f = sq % 8;

    int dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // NE, NW, SE, SW

    for (auto &d : dirs)
    {
        int i = r + d[0];
        int j = f + d[1];
        while (i >= 0 && i < NUM_ROWS && j >= 0 && j < NUM_COLS)
        {
            int targetSq = i * 8 + j;
            U64 targetMask = 1ULL << targetSq;

            if (friendly & targetMask)
                break;
            moves.push_back({sq, targetSq, BISHOP});

            if (enemy & targetMask)
                break;
            i += d[0];
            j += d[1];
        }
    }

    return moves;
}

std::vector<Move> MoveGen::generateBishopMoves(U64 bishops, U64 friendly, U64 enemy)
{
    std::vector<Move> moves;

    while (bishops)
    {
        int sq = __builtin_ctzll(bishops);
        bishops &= bishops - 1;
        auto sqMoves = bishopMovesFromSquare(sq, friendly, enemy);
        moves.insert(moves.end(), sqMoves.begin(), sqMoves.end());
    }

    return moves;
}

std::vector<Move> rookMovesFromSquare(int sq, U64 friendly, U64 enemy)
{
    std::vector<Move> moves;
    int r = sq / 8;
    int f = sq % 8;

    // Directions: N, S, E, W
    int dirs[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    for (auto &d : dirs)
    {
        int i = r + d[0];
        int j = f + d[1];
        while (i >= 0 && i < 8 && j >= 0 && j < 8)
        {
            int targetSq = i * 8 + j;
            U64 targetMask = 1ULL << targetSq;

            if (friendly & targetMask)
                break;

            moves.push_back({sq, targetSq, ROOK});

            if (enemy & targetMask)
                break;

            i += d[0];
            j += d[1];
        }
    }

    return moves;
}

std::vector<Move> MoveGen::generateRookMoves(U64 rooks, U64 friendly, U64 enemy)
{
    std::vector<Move> moves;

    while (rooks)
    {
        int sq = __builtin_ctzll(rooks);
        rooks &= rooks - 1;
        auto sqMoves = rookMovesFromSquare(sq, friendly, enemy);
        moves.insert(moves.end(), sqMoves.begin(), sqMoves.end());
    }
    return moves;
}

std::vector<Move> MoveGen::generateQueenMoves(U64 queens, U64 friendly, U64 enemy)
{
    std::vector<Move> moves;

    while (queens)
    {
        int sq = __builtin_ctzll(queens);
        queens &= queens - 1;

        auto bishopMoves = bishopMovesFromSquare(sq, friendly, enemy);
        auto rookMoves = rookMovesFromSquare(sq, friendly, enemy);

        moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
        moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    }

    return moves;
}

Board MoveGen::applyMove(Board b, Move m)
{
    if (b.getWhiteToMove())
    {
        if (m.kingsideCastle)
        {
            b.clearSquare(4);
            b.setPiece(KING, WHITE, 6);

            b.clearSquare(7);
            b.setPiece(ROOK, WHITE, 5);
        }
        else if (m.queensideCastle)
        {
            b.clearSquare(4);
            b.setPiece(KING, WHITE, 2);

            b.clearSquare(0);
            b.setPiece(ROOK, WHITE, 3);
        }
        else if (m.isEnPassant)
        {
            b.clearSquare(m.from);
            b.setPiece(PAWN, WHITE, m.to);

            int capturedSquare = m.to - 8;
            b.clearSquare(capturedSquare);
        }
        else
        {
            b.clearSquare(m.from);
            b.clearSquare(m.to);
            b.setPiece(m.piece, WHITE, m.to);
        }
    }
    else
    {
        if (m.kingsideCastle)
        {
            b.clearSquare(60);
            b.setPiece(KING, BLACK, 62);

            b.clearSquare(63);
            b.setPiece(ROOK, BLACK, 61);
        }
        else if (m.queensideCastle)
        {
            b.clearSquare(60);
            b.setPiece(KING, BLACK, 58);

            b.clearSquare(56);
            b.setPiece(ROOK, BLACK, 59);
        }
        else if (m.isEnPassant)
        {
            b.clearSquare(m.from);
            b.setPiece(PAWN, BLACK, m.to);

            int capturedSq = m.to + 8;
            b.clearSquare(capturedSq);
        }
        else
        {
            b.clearSquare(m.from);
            b.clearSquare(m.to);
            b.setPiece(m.piece, BLACK, m.to);
        }
    }
    b.setWhiteToMove(!b.getWhiteToMove());
    return b;
}
