#pragma once
#include <cstdint>
#include <vector>
#include <array>

#define TOTAL_SQUARES 64
#define NUM_ROWS 8
#define NUM_COLS 8

using U64 = std::uint64_t;

class Board
{
public:
    enum COLOR
    {
        WHITE,
        BLACK,
        BOTH
    };
    Board();
    void loadStartPosition();
    void clearBoard();
    void printBoard() const;

private:
    std::array<U64, 3> pawns{};
    std::array<U64, 3> knights{};
    std::array<U64, 3> bishops{};
    std::array<U64, 3> rooks{};
    std::array<U64, 3> queens{};
    std::array<U64, 3> kings{};
    std::array<U64, 3> occupancy{};

    bool castlingRights[4]; // WK, WQ, BK, BQ
    bool whiteToMove;
    int enPassantSquare;
    int halfMoveClock;
    int fullMoveNumber;
};