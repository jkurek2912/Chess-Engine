#include "Board.h"

#include <cstdint>
#include <vector>
#include <iostream>

Board::Board()
{
    clearBoard();
    loadStartPosition();
}

void Board::clearBoard()
{
    for (auto &it : pawns)
        it = 0;
    for (auto &it : knights)
        it = 0;
    for (auto &it : bishops)
        it = 0;
    for (auto &it : rooks)
        it = 0;
    for (auto &it : queens)
        it = 0;
    for (auto &it : kings)
        it = 0;
    for (auto &it : occupancy)
        it = 0;

    castlingRights[0] = castlingRights[1] = castlingRights[2] = castlingRights[3] = false;
    enPassantSquare = -1;
    halfMoveClock = 0;
    fullMoveNumber = 0;
    whiteToMove = true;
}

void Board::loadStartPosition()
{
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

    setOccupancy();

    castlingRights[0] = castlingRights[1] = castlingRights[2] = castlingRights[3] = false;
    enPassantSquare = -1;
    halfMoveClock = 0;
    fullMoveNumber = 0;
    whiteToMove = true;
}

void placePieces(std::vector<std::vector<char>> &board, std::array<U64, 3> bitboard, char whiteChar, char blackChar)
{
    for (int i = 0; i < 2; i++)
    {
        while (bitboard[i])
        {
            int sq = __builtin_ctzll(bitboard[i]); // finds position of LSB
            bitboard[i] &= (bitboard[i] - 1);      // clears LSB bit

            int row = sq / 8;
            int col = sq % 8;

            board[row][col] = (i == WHITE) ? whiteChar : blackChar;
        }
    }
}

void Board::printBoard() const
{
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    placePieces(board, pawns, 'P', 'p');
    placePieces(board, knights, 'N', 'n');
    placePieces(board, bishops, 'B', 'b');
    placePieces(board, rooks, 'R', 'r');
    placePieces(board, queens, 'Q', 'q');
    placePieces(board, kings, 'K', 'k');
    for (int i = NUM_ROWS - 1; i >= 0; i--)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            std::cout << board[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

void Board::setOccupancy()
{
    occupancy[WHITE] = pawns[WHITE] | knights[WHITE] | bishops[WHITE] | rooks[WHITE] | queens[WHITE] | kings[WHITE];
    occupancy[BLACK] = pawns[BLACK] | knights[BLACK] | bishops[BLACK] | rooks[BLACK] | queens[BLACK] | kings[BLACK];
    occupancy[BOTH] = occupancy[WHITE] | occupancy[BLACK];
}

inline void Board::clearSquare(int sq)
{
    U64 mask = ~(1ULL << sq);
    for (auto &it : pawns)
        it &= mask;
    for (auto &it : knights)
        it &= mask;
    for (auto &it : bishops)
        it &= mask;
    for (auto &it : rooks)
        it &= mask;
    for (auto &it : queens)
        it &= mask;
    for (auto &it : kings)
        it &= mask;
    for (auto &it : occupancy)
        it &= mask;

    setOccupancy();
}

void Board::setPiece(PIECE piece, COLOR color, int sq)
{
    U64 mask = (1ULL << sq);

    switch (piece)
    {
    case PAWN:
        (color == WHITE ? pawns[WHITE] : pawns[BLACK]) |= mask;
        break;
    case KNIGHT:
        (color == WHITE ? knights[WHITE] : knights[BLACK]) |= mask;
        break;
    case BISHOP:
        (color == WHITE ? bishops[WHITE] : bishops[BLACK]) |= mask;
        break;
    case ROOK:
        (color == WHITE ? rooks[WHITE] : rooks[BLACK]) |= mask;
        break;
    case QUEEN:
        (color == WHITE ? queens[WHITE] : queens[BLACK]) |= mask;
        break;
    case KING:
        (color == WHITE ? kings[WHITE] : kings[BLACK]) |= mask;
        break;
    default:
        break;
    }
    occupancy[color] |= mask;
    occupancy[BOTH] |= mask;
}

void Board::customSetBoard()
{
    Board::clearBoard();
    std::vector<std::vector<char>> customBoard = {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                                                  {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'.', '.', '.', '.', '.', '.', '.', '.'},
                                                  {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                                                  {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

    castlingRights[0] = castlingRights[1] = castlingRights[2] = castlingRights[3] = false;
    enPassantSquare = -1;
    halfMoveClock = 0;
    fullMoveNumber = 0;
    whiteToMove = true;

    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            char piece = customBoard[i][j];
            int pos = (7 - i) * 8 + j;
            switch (piece)
            {
            case 'P':
                pawns[WHITE] |= (1ULL << pos);
                break;
            case 'N':
                knights[WHITE] |= (1ULL << pos);
                break;
            case 'B':
                bishops[WHITE] |= (1ULL << pos);
                break;
            case 'R':
                rooks[WHITE] |= (1ULL << pos);
                break;
            case 'Q':
                queens[WHITE] |= (1ULL << pos);
                break;
            case 'K':
                kings[WHITE] |= (1ULL << pos);
                break;
            case 'p':
                pawns[BLACK] |= (1ULL << pos);
                break;
            case 'n':
                knights[BLACK] |= (1ULL << pos);
                break;
            case 'b':
                bishops[BLACK] |= (1ULL << pos);
                break;
            case 'r':
                rooks[BLACK] |= (1ULL << pos);
                break;
            case 'q':
                queens[BLACK] |= (1ULL << pos);
                break;
            case 'k':
                kings[BLACK] |= (1ULL << pos);
                break;
            }
        }
    }
    pawns[BOTH] = pawns[WHITE] | pawns[BLACK];
    knights[BOTH] = knights[WHITE] | knights[BLACK];
    bishops[BOTH] = bishops[WHITE] | bishops[BLACK];
    rooks[BOTH] = rooks[WHITE] | rooks[BLACK];
    queens[BOTH] = queens[WHITE] | queens[BLACK];
    kings[BOTH] = kings[WHITE] | kings[BLACK];

    setOccupancy();
}
