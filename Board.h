#pragma once
#include <cstdint>
#include <vector>
#include <array>

#define TOTAL_SQUARES 64
#define NUM_ROWS 8
#define NUM_COLS 8

enum COLOR
{
    WHITE,
    BLACK,
    BOTH
};

using U64 = std::uint64_t;

class Board
{
public:
    Board();
    void loadStartPosition();
    void clearBoard();
    void customSetBoard();
    void printBoard() const;
    void setOccupancy();

    U64 getPawns(COLOR c) const { return pawns[c]; }
    U64 getBishops(COLOR c) const { return bishops[c]; }
    U64 getKnights(COLOR c) const { return knights[c]; }
    U64 getRooks(COLOR c) const { return rooks[c]; }
    U64 getQueens(COLOR c) const { return queens[c]; }
    U64 getKings(COLOR c) const { return kings[c]; }
    U64 getOccupancy(COLOR c) const { return occupancy[c]; }

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