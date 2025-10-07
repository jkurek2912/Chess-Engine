#include "Board.h"
#include <iostream>

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

    queens[WHITE] = 0x0000000000000010ULL;
    queens[BLACK] = 0x1000000000000000ULL;
    queens[BOTH] = queens[WHITE] | queens[BLACK];

    kings[WHITE] = 0x0000000000000008ULL;
    kings[BLACK] = 0x0800000000000000ULL;
    kings[BOTH] = kings[WHITE] | kings[BLACK];

    occupancy[WHITE] = pawns[WHITE] | knights[WHITE] | bishops[WHITE] | rooks[WHITE] | queens[WHITE] | kings[WHITE];
    occupancy[BLACK] = pawns[BLACK] | knights[BLACK] | bishops[BLACK] | rooks[BLACK] | queens[BLACK] | kings[BLACK];
    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];

    castlingRights = {true, true, true, true};
    moves = 0;
    movesSinceCapture = 0;
    enPassantSquare = -1;

    whiteToMove = true;
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

void Board::printBoard()
{
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    std::vector<std::pair<std::array<uint64_t, 3> *, Piece>> allPieces = {
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

void Board::setPiece(Piece piece, Color color, int square)
{
    clearSquare(piece, color, square);
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

void Board::setCustomBoard()
{
    clearBoard();
    std::vector<std::vector<char>> customBoard = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                                                  {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                                                  {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

    whiteToMove = true;

    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            char c = customBoard[i][j];
            if (c == '.')
                continue;

            auto [piece, color] = charToPiece[c];
            int index = rowColToIndex(i, j);
            setPiece(piece, color, index);
        }
    }
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

std::pair<Piece, Color> Board::findPiece(int square)
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