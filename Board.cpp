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
            int sq = __builtin_ctzll(bitboard[i]);  // finds LSB
            bitboard[i] &= (bitboard[i] - 1);       // clears LSB bit

            int row = sq / 8;
            int col = sq % 8;

            board[row][col] = (i == Board::WHITE) ? whiteChar : blackChar;
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
