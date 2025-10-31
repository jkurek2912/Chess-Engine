#include "Board.h"
#include "MoveGen.h"
#include "Zobrist.h"
#include <iostream>
#include <stdexcept>

std::map<std::pair<Piece, Color>, char> pieceToChar = {
    {{PAWN, WHITE}, 'P'},
    {{KNIGHT, WHITE}, 'N'},
    {{BISHOP, WHITE}, 'B'},
    {{ROOK, WHITE}, 'R'},
    {{QUEEN, WHITE}, 'Q'},
    {{KING, WHITE}, 'K'},
    {{PAWN, BLACK}, 'p'},
    {{KNIGHT, BLACK}, 'n'},
    {{BISHOP, BLACK}, 'b'},
    {{ROOK, BLACK}, 'r'},
    {{QUEEN, BLACK}, 'q'},
    {{KING, BLACK}, 'k'}};

std::map<char, std::pair<Piece, Color>> charToPiece = {
    {'P', {PAWN, WHITE}},
    {'N', {KNIGHT, WHITE}},
    {'B', {BISHOP, WHITE}},
    {'R', {ROOK, WHITE}},
    {'Q', {QUEEN, WHITE}},
    {'K', {KING, WHITE}},
    {'p', {PAWN, BLACK}},
    {'n', {KNIGHT, BLACK}},
    {'b', {BISHOP, BLACK}},
    {'r', {ROOK, BLACK}},
    {'q', {QUEEN, BLACK}},
    {'k', {KING, BLACK}}};

int rowColToIndex(int row, int col)
{
    return (7 - row) * 8 + col;
}

std::pair<int, int> indexToRowCol(int index)
{
    int row = index / 8;
    int col = index % 8;
    return {row, col};
}

void Board::setBoard()
{
    clearBoard();
    pawns[WHITE] = 0x000000000000FF00ULL;
    pawns[BLACK] = 0x00FF000000000000ULL;
    pawns[BOTH] = pawns[WHITE] | pawns[BLACK];

    knights[WHITE] = 0x0000000000000042ULL;
    knights[BLACK] = 0x4200000000000000ULL;
    knights[BOTH] = knights[WHITE] | knights[BLACK];

    bishops[WHITE] = 0x0000000000000024ULL;
    bishops[BLACK] = 0x2400000000000000ULL;
    bishops[BOTH] = bishops[WHITE] | bishops[BLACK];

    rooks[WHITE] = 0x0000000000000081ULL;
    rooks[BLACK] = 0x8100000000000000ULL;
    rooks[BOTH] = rooks[WHITE] | rooks[BLACK];

    queens[WHITE] = 0x0000000000000008ULL;
    queens[BLACK] = 0x0800000000000000ULL;
    queens[BOTH] = queens[WHITE] | queens[BLACK];

    kings[WHITE] = 0x0000000000000010ULL;
    kings[BLACK] = 0x1000000000000000ULL;
    kings[BOTH] = kings[WHITE] | kings[BLACK];

    occupancy[WHITE] = pawns[WHITE] | knights[WHITE] | bishops[WHITE] | rooks[WHITE] | queens[WHITE] | kings[WHITE];
    occupancy[BLACK] = pawns[BLACK] | knights[BLACK] | bishops[BLACK] | rooks[BLACK] | queens[BLACK] | kings[BLACK];
    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    castlingMask = 0b1111;
    halfMoveClock = 0;
    moves = 0;
    enPassantSquare = -1;

    whiteToMove = true;

    hash = computeZobrist();
    repetitionCount.clear();
    repetitionCount[hash] = 1;
}

void Board::clearBoard()
{
    pawns[WHITE] = 0;
    pawns[BLACK] = 0;
    pawns[BOTH] = 0;

    knights[WHITE] = 0;
    knights[BLACK] = 0;
    knights[BOTH] = 0;

    bishops[WHITE] = 0;
    bishops[BLACK] = 0;
    bishops[BOTH] = 0;

    rooks[WHITE] = 0;
    rooks[BLACK] = 0;
    rooks[BOTH] = 0;

    queens[WHITE] = 0;
    queens[BLACK] = 0;
    queens[BOTH] = 0;

    kings[WHITE] = 0;
    kings[BLACK] = 0;
    kings[BOTH] = 0;

    occupancy[WHITE] = 0;
    occupancy[BLACK] = 0;
    occupancy[BOTH] = 0;

    hash = 0;
    repetitionCount.clear();
    repetitionCount[hash] = 0;
}

void setPieces(uint64_t bitboard, Piece piece, Color color, std::vector<std::vector<char>> &board)
{
    while (bitboard > 0)
    {
        int index = __builtin_ctzll(bitboard);
        bitboard &= (bitboard - 1);
        auto [row, col] = indexToRowCol(index);
        board[row][col] = pieceToChar[{piece, color}];
    }
}

void Board::printBoard() const
{
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    std::vector<std::pair<const std::array<uint64_t, 3> *, Piece>> allPieces = {
        {&pawns, PAWN},
        {&knights, KNIGHT},
        {&bishops, BISHOP},
        {&rooks, ROOK},
        {&queens, QUEEN},
        {&kings, KING}};

    for (auto &entry : allPieces)
    {
        auto &bitboards = *entry.first;
        Piece piece = entry.second;

        setPieces(bitboards[WHITE], piece, WHITE, board);
        setPieces(bitboards[BLACK], piece, BLACK, board);
    }

    for (int i = NUM_ROWS - 1; i >= 0; i--)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }
}

bool Board::isDraw() const
{
    if (halfMoveClock >= 100)
        return true;

    auto it = repetitionCount.find(hash);
    if (it != repetitionCount.end() && it->second >= 3)
        return true;

    return false;
}

void Board::setPiece(Piece piece, Color color, int square)
{
    uint64_t mask = (1ULL << square);

    switch (piece)
    {
    case PAWN:
        pawns[color] |= mask;
        pawns[BOTH] |= mask;
        break;
    case KNIGHT:
        knights[color] |= mask;
        knights[BOTH] |= mask;
        break;
    case BISHOP:
        bishops[color] |= mask;
        bishops[BOTH] |= mask;
        break;
    case ROOK:
        rooks[color] |= mask;
        rooks[BOTH] |= mask;
        break;
    case QUEEN:
        queens[color] |= mask;
        queens[BOTH] |= mask;
        break;
    case KING:
        kings[color] |= mask;
        kings[BOTH] |= mask;
        break;
    default:
        break;
    }

    occupancy[color] |= mask;
    occupancy[BOTH] |= mask;
}

void Board::setCustomBoard(const std::string &fen)
{
    clearBoard();

    std::istringstream iss(fen);
    std::string piecePlacement, sideToMove, castling, enPassant;
    int halfmoveClock = 0, moves = 1;

    iss >> piecePlacement >> sideToMove >> castling >> enPassant >> halfmoveClock >> moves;

    if (piecePlacement.empty() || sideToMove.empty() || castling.empty() || enPassant.empty())
    {
        throw std::invalid_argument("FEN missing required fields");
    }

    if (sideToMove != "w" && sideToMove != "b")
    {
        throw std::invalid_argument("Invalid side to move (must be 'w' or 'b')");
    }

    if (castling != "-")
    {
        for (char c : castling)
        {
            if (c != 'K' && c != 'Q' && c != 'k' && c != 'q')
            {
                throw std::invalid_argument("Invalid castling rights format");
            }
        }
    }

    if (enPassant != "-")
    {
        if (enPassant.length() != 2)
        {
            throw std::invalid_argument("Invalid en passant square format");
        }
        if (enPassant[0] < 'a' || enPassant[0] > 'h')
        {
            throw std::invalid_argument("Invalid en passant file");
        }
        if (enPassant[1] < '1' || enPassant[1] > '8')
        {
            throw std::invalid_argument("Invalid en passant rank");
        }
        if (enPassant[1] != '3' && enPassant[1] != '6')
        {
            throw std::invalid_argument("En passant square must be on rank 3 or 6");
        }
    }

    int rowCount = 0;
    int squareCount = 0;
    int square = 56;
    int whiteKings = 0;
    int blackKings = 0;

    for (char c : piecePlacement)
    {
        if (c == '/')
        {
            if (squareCount != 8)
            {
                throw std::invalid_argument("Invalid row in piece placement (must have 8 squares)");
            }
            rowCount++;
            square -= 16;
            squareCount = 0;
        }
        else if (isdigit(c))
        {
            int num = c - '0';
            if (num < 1 || num > 8)
            {
                throw std::invalid_argument("Invalid digit in piece placement");
            }
            square += num;
            squareCount += num;
        }
        else
        {
            if (charToPiece.find(c) == charToPiece.end())
            {
                throw std::invalid_argument("Invalid piece character in FEN");
            }

            auto [piece, color] = charToPiece[c];

            int row = square / 8;
            if (piece == PAWN && (row == 0 || row == 7))
            {
                throw std::invalid_argument("Pawns cannot be on first or last rank");
            }

            if (piece == KING)
            {
                if (color == WHITE)
                    whiteKings++;
                else
                    blackKings++;
            }

            if (square < 0 || square > 63)
            {
                throw std::invalid_argument("Square index out of bounds");
            }

            setPiece(piece, color, square);
            square++;
            squareCount++;
        }
    }

    if (squareCount != 8)
    {
        throw std::invalid_argument("Invalid row in piece placement (must have 8 squares)");
    }
    if (rowCount != 7)
    {
        throw std::invalid_argument("Piece placement must have exactly 8 rows");
    }

    if (whiteKings != 1)
    {
        throw std::invalid_argument("Must have exactly one white king");
    }
    if (blackKings != 1)
    {
        throw std::invalid_argument("Must have exactly one black king");
    }

    if (halfmoveClock < 0)
    {
        throw std::invalid_argument("Halfmove clock cannot be negative");
    }
    if (moves < 1)
    {
        throw std::invalid_argument("Fullmove number must be at least 1");
    }

    whiteToMove = (sideToMove == "w");

    castlingMask = 0b0000;
    if (castling.find('K') != std::string::npos)
        castlingMask |= (1 << WHITE_KING);
    if (castling.find('Q') != std::string::npos)
        castlingMask |= (1 << WHITE_QUEEN);
    if (castling.find('k') != std::string::npos)
        castlingMask |= (1 << BLACK_KING);
    if (castling.find('q') != std::string::npos)
        castlingMask |= (1 << BLACK_QUEEN);
    if (enPassant != "-")
    {
        int file = enPassant[0] - 'a';
        int rank = enPassant[1] - '1';
        enPassantSquare = rank * 8 + file;
    }
    else
        enPassantSquare = -1;

    occupancy[WHITE] = pawns[WHITE] | knights[WHITE] | bishops[WHITE] | rooks[WHITE] | queens[WHITE] | kings[WHITE];
    occupancy[BLACK] = pawns[BLACK] | knights[BLACK] | bishops[BLACK] | rooks[BLACK] | queens[BLACK] | kings[BLACK];
    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    this->halfMoveClock = halfmoveClock;
    this->moves = moves;

    hash = computeZobrist();
    repetitionCount.clear();
    repetitionCount[hash] = 1;
}

void Board::clearSquare(Piece piece, Color color, int square)
{
    uint64_t mask = ~(1ULL << square);

    switch (piece)
    {
    case PAWN:
        pawns[color] &= mask;
        pawns[BOTH] &= mask;
        break;
    case KNIGHT:
        knights[color] &= mask;
        knights[BOTH] &= mask;
        break;
    case BISHOP:
        bishops[color] &= mask;
        bishops[BOTH] &= mask;
        break;
    case ROOK:
        rooks[color] &= mask;
        rooks[BOTH] &= mask;
        break;
    case QUEEN:
        queens[color] &= mask;
        queens[BOTH] &= mask;
        break;
    case KING:
        kings[color] &= mask;
        kings[BOTH] &= mask;
        break;
    default:
        break;
    }

    occupancy[color] &= mask;
    occupancy[BOTH] &= mask;
}

std::pair<Piece, Color> Board::findPiece(int square) const
{
    uint64_t mask = (1ULL << square);

    if (pawns[WHITE] & mask)
        return {PAWN, WHITE};
    if (knights[WHITE] & mask)
        return {KNIGHT, WHITE};
    if (bishops[WHITE] & mask)
        return {BISHOP, WHITE};
    if (rooks[WHITE] & mask)
        return {ROOK, WHITE};
    if (queens[WHITE] & mask)
        return {QUEEN, WHITE};
    if (kings[WHITE] & mask)
        return {KING, WHITE};

    if (pawns[BLACK] & mask)
        return {PAWN, BLACK};
    if (knights[BLACK] & mask)
        return {KNIGHT, BLACK};
    if (bishops[BLACK] & mask)
        return {BISHOP, BLACK};
    if (rooks[BLACK] & mask)
        return {ROOK, BLACK};
    if (queens[BLACK] & mask)
        return {QUEEN, BLACK};
    if (kings[BLACK] & mask)
        return {KING, BLACK};

    return {NONE, BOTH};
}

void Board::updateZobrist(const Move &move, const MoveState &state) noexcept
{
    hash ^= zobristSide;

    const int from = move.from;
    const int to = move.to;
    const Color color = move.color;

    const Piece pieceFrom = move.isPromotion ? PAWN : move.piece;
    const Piece pieceTo = move.isPromotion ? move.piece : pieceFrom;

    hash ^= zobristPiece[color][pieceFrom][from];
    hash ^= zobristPiece[color][pieceTo][to];

    if (move.isCapture) [[likely]]
    {
        const Piece captured = state.capturedPiece;
        if (captured != NONE)
            hash ^= zobristPiece[state.capturedColor][captured][state.capturedSquare];
    }

    if (move.isCastle) [[unlikely]]
    {
        static constexpr int rookFromTo[2][2][2] = {
            {{7, 5}, {0, 3}},
            {{63, 61}, {56, 59}}};
        int side = (to == 6 || to == 62) ? 0 : 1;
        int clr = (color == WHITE ? 0 : 1);
        hash ^= zobristPiece[color][ROOK][rookFromTo[clr][side][0]];
        hash ^= zobristPiece[color][ROOK][rookFromTo[clr][side][1]];
    }

    uint8_t diff = state.castlingMask ^ castlingMask;
    if (diff) [[unlikely]]
    {
        if (diff & 1)
            hash ^= zobristCastling[0];
        if (diff & 2)
            hash ^= zobristCastling[1];
        if (diff & 4)
            hash ^= zobristCastling[2];
        if (diff & 8)
            hash ^= zobristCastling[3];
    }

    const int oldEp = state.enPassantSquare;
    const int newEp = enPassantSquare;
    if (oldEp != -1)
        hash ^= zobristEnPassant[oldEp & 7];
    if (newEp != -1)
        hash ^= zobristEnPassant[newEp & 7];
}

uint64_t Board::computeZobrist() const
{
    uint64_t h = 0;

    auto addPieces = [&](uint64_t bb, int piece, int color)
    {
        while (bb)
        {
            int sq = __builtin_ctzll(bb);
            h ^= zobristPiece[color][piece][sq];
            bb &= bb - 1;
        }
    };

    addPieces(pawns[WHITE], PAWN, WHITE);
    addPieces(pawns[BLACK], PAWN, BLACK);
    addPieces(knights[WHITE], KNIGHT, WHITE);
    addPieces(knights[BLACK], KNIGHT, BLACK);
    addPieces(bishops[WHITE], BISHOP, WHITE);
    addPieces(bishops[BLACK], BISHOP, BLACK);
    addPieces(rooks[WHITE], ROOK, WHITE);
    addPieces(rooks[BLACK], ROOK, BLACK);
    addPieces(queens[WHITE], QUEEN, WHITE);
    addPieces(queens[BLACK], QUEEN, BLACK);
    addPieces(kings[WHITE], KING, WHITE);
    addPieces(kings[BLACK], KING, BLACK);

    if (castlingMask & (1 << WHITE_KING))
        h ^= zobristCastling[0];
    if (castlingMask & (1 << WHITE_QUEEN))
        h ^= zobristCastling[1];
    if (castlingMask & (1 << BLACK_KING))
        h ^= zobristCastling[2];
    if (castlingMask & (1 << BLACK_QUEEN))
        h ^= zobristCastling[3];

    if (enPassantSquare != -1)
        h ^= zobristEnPassant[enPassantSquare % 8];

    if (whiteToMove)
        h ^= zobristSide;

    return h;
}

std::string Move::moveToString(const Move &m)
{
    const char *files = "abcdefgh";
    int fromFile = m.from % 8, fromRank = m.from / 8;
    int toFile = m.to % 8, toRank = m.to / 8;

    std::string s;
    s += files[fromFile];
    s += std::to_string(fromRank + 1);
    s += files[toFile];
    s += std::to_string(toRank + 1);
    return s;
}