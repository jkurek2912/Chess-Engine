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

void Board::printBoard() const {
    std::vector<std::vector<char>> board(8, std::vector<char>(8, '.'));
    for (int i = 0; i < 2; i++) {
        U64 p = pawns[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'P';
                } else {
                    board[row][col] = 'p';
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        U64 p = knights[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'N';
                } else {
                    board[row][col] = 'n';
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        U64 p = bishops[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'B';
                } else {
                    board[row][col] = 'b';
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        U64 p = rooks[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'R';
                } else {
                    board[row][col] = 'r';
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        U64 p = queens[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'Q';
                } else {
                    board[row][col] = 'q';
                }
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        U64 p = kings[i];
        for (int j = 0; j < TOTAL_SQUARES; j++) {
            if (p & (1ULL << j)) {
                int row = j / 8;
                int col = j % 8;
                if (i == 0) {
                    board[row][col] = 'K';
                } else {
                    board[row][col] = 'k';
                }
            }
        }
    }
    
    for (int i = NUM_ROWS - 1; i >= 0; i--) {
        for (int j = NUM_COLS - 1; j >= 0; j--) {
            std::cout << board[i][j] << ' ';
        }
        std::cout << '\n';
    }
}
